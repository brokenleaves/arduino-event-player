#include "HardwareSerial.h"
#include "MsTimer2.h"
#include "DynamicMemory.h"
#include "LedArray.h"
#include "EventPlayer.h"
#include "LedArrayEvent.h"
#include "SimpleSerialProtocol.h"

/*
	Serial protocol commands:

	LEDARRAY,<ledCount>,<ledCycleDurationMicros>
		ledCount = integer
		ledCycleDurationMicros = PWM cycle duration in microseconds (has to be >= 500 µs)
	LEDPIN,<ledIndex>,<pinNumber>
		ledIndex = ID number of LED starting from zero
		pinNumber = Arduino hardware pin number of LED starting from zero
	EVENTPLR,<eventCount>,<continuousEventCount>
		eventCount = maximum number of events in player buffer
		continuousEventCount = maximum number of simultaneous fade (= continuous) events
		  (usually the same as ledCount)
	<event>,<startTimeMillis>,...
		startTimeMillis = start time in milliseconds for the event relative
		  to the start time of event player
		event = ESET, ESETREL, EFADE, EFADEREL, EJUMP
		
		EJUMP,<startTimeMillis>,<relativeEventIndex>,<loopCount>
			relativeEventIndex = index of the event to jump to
			  relative to the current event index
			loopCount = number of times to execute jump, negative for infinite
		ESET,<startTimeMillis>,<ledIndex>,<brightness>
			brightness = time of the PWM cycle in microseconds the LED is on
			  (0 = constantly off, <ledCycleDurationMicros> = constantly on)
		ESETREL,<startTimeMillis>,<ledIndex>,<relativeBrightness>
			brightness = time of the PWM cycle in microseconds the LED is on
			  -- the value is added to the current brightness (can be negative)
			  (0 = constantly off, <ledCycleDurationMicros> = constantly on)
		EFADE,<startTimeMillis>,<lengthMillis>,<ledIndex>,<fromBrightness>,<toBrightness>
			lengthMillis = length of fade in milliseconds
			fromBrightness = start fade with the given brightness level
			toBrightness = end fade with the given brightness level
		EFADETO,<startTimeMillis>,<lengthMillis>,<ledIndex>,<toBrightness>
			toBrightness = end fade with the given brightness level,
			  start fade from the current brightness level at the start of the event
		EFADEREL,<startTimeMillis>,<lengthMillis>,<ledIndex>,<relativeBrightness>
			relativeBrightness = end fade with the given brightness level
			  relative to the current brightness at the start of event
*/

/*
LEDARRAY,16,6000;
EVENTPLR,100,16;
LEDPIN,0,2;
LEDPIN,1,3;
LEDPIN,2,4;
LEDPIN,3,13;
EFADETO,0000,5000,0,6000;
EFADETO,1000,5000,1,6000;
EFADETO,2000,5000,2,6000;
EFADETO,3000,5000,3,6000;
EFADETO,6000,5000,0,0000;
EFADETO,7000,5000,1,0000;
EFADETO,8000,5000,2,0000;
EFADETO,9000,5000,3,0000;
START;

*/

  /*player->add(new FadeToLedBrightnessEvent(1000, 8999, 3, LED_MAX));
  player->add(new FadeToLedBrightnessEvent(1000, 4000, 0, LED_MAX));
  player->add(new SetLedBrightnessEvent(2000, 1, LED_MAX));
  player->add(new SetLedBrightnessEvent(3000, 2, LED_MAX));

  player->add(new FadeToLedBrightnessEvent(5000, 2000, 1, 0));
  player->add(new SetLedBrightnessEvent(6000, 0, 0));
  player->add(new SetRelativeLedBrightnessEvent(7000, 2, -(LED_MAX * 0.75)));
  player->add(new FadeToLedBrightnessEvent(10000, 4000, 0, LED_MAX));
  player->add(new FadeToLedBrightnessEvent(10000, 4000, 1, LED_MAX));
  player->add(new FadeToLedBrightnessEvent(10000, 4000, 2, LED_MAX));
  player->add(new FadeLedBrightnessEvent(10000, 5000, 3, LED_MAX * 0.5, 0));

  player->add(new SetLedBrightnessEvent(15000, 0, 0));
  player->add(new SetLedBrightnessEvent(15000, 1, 0));
  player->add(new SetLedBrightnessEvent(15000, 2, 0));

  player->add(new JumpEvent(17000, 0, -1));*/

#define LED_ARRAY_SIZE                   16
#define LED_CYCLE_DURATION_MICROS        ((unsigned long) 6000)
#define LED_MAX                          LED_CYCLE_DURATION_MICROS

char ledPins[] = { 2, 3, 4, 13, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1 };

static SimpleSerialProtocol *ssp = NULL;
static LedArray *ledArray = NULL;
static EventPlayer *player = NULL;

static volatile boolean started = false;

void run() {
  unsigned long time = micros();
  player->run(time);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Event-based light PWM player initializing...");

  ssp = new SimpleSerialProtocol();

  Serial.println("Application waiting for commands.");
}

boolean checkCommand(SimpleSerialCommand *cmd, char *name, unsigned char paramCount) {
  if (cmd->count != paramCount) {
    Serial.print("ERROR: ");
    Serial.print(name);
    Serial.print(" command requires ");
    Serial.print(paramCount, DEC);
    Serial.println(" parameters");
    return false;
  }

  return true;
}

boolean checkIfNull(void *ptr, char *name) {
  if (ptr != NULL) {
    Serial.print("ERROR: ");
    Serial.print(name);
    Serial.println(" is already initialized");
    return false;
  }

  return true;
}

boolean processCommand(SimpleSerialCommand *cmd) {
  if (strcmp("LEDARRAY", cmd->name) == 0) {
    if (!checkCommand(cmd, "LEDARRAY", 2)) {
      return false;
    }
    if (!checkIfNull((void *) ledArray, "LEDARRAY")) {
      return false;
    }

    ledArray = new LedArray(cmd->parameter[0], cmd->parameter[1]);
    LedArrayConfig::ledArray = ledArray;
  } else if (strcmp("LEDPIN", cmd->name) == 0) {
    if (!checkCommand(cmd, "LEDPIN", 2)) {
      return false;
    }
    if (ledArray == NULL) {
      Serial.println("ERROR: LEDARRAY not initialized yet");
      return false;
    }
 
    char index = (char) cmd->parameter[0];
    char pin = (char) cmd->parameter[1];
    if ((index < 0) || (index >= ledArray->getLedCount())) {
      Serial.print("ERROR: Led index must be an integer between 0 and ");
      Serial.println(ledArray->getLedCount(), DEC);
      return false;
    }
    if ((pin < 0) || (pin >= 20)) {
      Serial.println("ERROR: Led index must be an integer between 0 and 19");
      return false;
    }

    ledArray->setLedPin(index, pin);
  } else if (strcmp("EVENTPLR", cmd->name) == 0) {
    if (!checkCommand(cmd, "EVENTPLR", 2)) {
      return false;
    }
    if (!checkIfNull((void *) player, "EVENTPLR")) {
      return false;
    }

    player = new EventPlayer(cmd->parameter[0], cmd->parameter[1]);
  } else if (strcmp("START", cmd->name) == 0) {
    if ((ledArray == NULL) || (player == NULL)) {
      Serial.println("ERROR: LEDARRAY and EVENTPLR must be initialized before START");
      return false;
    }

    player->prepare();
  
    MsTimer2::set(5, run);
    MsTimer2::start();
  
    player->start(micros());

    started = true;
  } else if (strcmp("EJUMP", cmd->name) == 0) {
    if (!checkCommand(cmd, "EJUMP", 3)) {
      return false;
    }

    player->add(new JumpEvent(cmd->parameter[0], cmd->parameter[1], cmd->parameter[2]));
  } else if (strcmp("ESET", cmd->name) == 0) {
    if (!checkCommand(cmd, "ESET", 3)) {
      return false;
    }

    player->add(new SetLedBrightnessEvent(cmd->parameter[0], cmd->parameter[1], cmd->parameter[2]));
  } else if (strcmp("ESETREL", cmd->name) == 0) {
    if (!checkCommand(cmd, "ESETREL", 3)) {
      return false;
    }

    player->add(new SetRelativeLedBrightnessEvent(cmd->parameter[0], cmd->parameter[1], cmd->parameter[2]));
  } else if (strcmp("EFADE", cmd->name) == 0) {
    if (!checkCommand(cmd, "EFADE", 5)) {
      return false;
    }

    player->add(new FadeLedBrightnessEvent(cmd->parameter[0], cmd->parameter[1], cmd->parameter[2], cmd->parameter[3], cmd->parameter[4]));
  } else if (strcmp("EFADETO", cmd->name) == 0) {
    if (!checkCommand(cmd, "EFADETO", 4)) {
      return false;
    }

    player->add(new FadeToLedBrightnessEvent(cmd->parameter[0], cmd->parameter[1], cmd->parameter[2], cmd->parameter[3]));
  } else if (strcmp("EFADEREL", cmd->name) == 0) {
    if (!checkCommand(cmd, "EFADEREL", 4)) {
      return false;
    }

    player->add(new FadeRelativeLedBrightnessEvent(cmd->parameter[0], cmd->parameter[1], cmd->parameter[2], cmd->parameter[3]));
  } else {
    Serial.print("ERROR: Unknown command: ");
    Serial.println(cmd->name);
    return false;
  }

  return true;
}

void loop() {
  if (started) {
    ledArray->update(micros());
  }

  if (Serial.available() > 0) {
    unsigned char data = (unsigned char) Serial.read();
    boolean valid = ssp->process(data);

    if (ssp->isError()) {
      Serial.println("ERROR: Invalid command syntax");
      ssp->reset();
      return;
    }
    if (!valid) {
      return;
    }
  
    SimpleSerialCommand *cmd = ssp->getReceivedCommand(); 
    processCommand(cmd);

    ssp->reset();
  }

  /*long endTime = micros();
  
  if (time % (unsigned long) 100000 == 0) {
    Serial.println(endTime - time, DEC);
    Serial.println();
  }*/
}

