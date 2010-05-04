#include "MsTimer2.h"
#include "DynamicMemory.h"
#include "LedArray.h"
#include "EventPlayer.h"
#include "LedArrayEvent.h"

/*
	Serial protocol commands:

	LEDARRAY <ledCount>,<ledCycleDurationMicros>
		ledCount = integer
		ledCycleDurationMicros = PWM cycle duration in microseconds (has to be >= 500 µs)
	LEDPIN <ledIndex>,<pinNumber>
		ledIndex = ID number of LED starting from zero
		pinNumber = Arduino hardware pin number of LED starting from zero
	EVENTPLAYER <eventCount>,<continuousEventCount>
		eventCount = maximum number of events in player buffer
		continuousEventCount = maximum number of simultaneous fade (= continuous) events
		  (usually the same as ledCount)
	EVENT <type>,<startTimeMillis>,...
		startTimeMillis = start time in milliseconds for the event relative
		  to the start time of event player
		type = SET, SETRELATIVE, FADE, FADERELATIVE
		
		EVENT SET,<startTimeMillis>,<ledIndex>,<brightness>
			brightness = time of the PWM cycle in microseconds the LED is on
			  (0 = constantly off, <ledCycleDurationMicros> = constantly on)
		EVENT SETREL,<startTimeMillis>,<ledIndex>,<relativeBrightness>
			brightness = time of the PWM cycle in microseconds the LED is on
			  -- the value is added to the current brightness (can be negative)
			  (0 = constantly off, <ledCycleDurationMicros> = constantly on)
		EVENT FADE,<startTimeMillis>,<lengthMillis>,<ledIndex>,<fromBrightness>,<toBrightness>
			lengthMillis = length of fade in milliseconds
			fromBrightness = start fade with the given brightness level
			toBrightness = end fade with the given brightness level
		EVENT FADETO,<startTimeMillis>,<lengthMillis>,<ledIndex>,<toBrightness>
			toBrightness = end fade with the given brightness level,
			  start fade from the current brightness level at the start of the event
		EVENT FADEREL,<startTimeMillis>,<lengthMillis>,<ledIndex>,<relativeBrightness>
			relativeBrightness = end fade with the given brightness level
			  relative to the current brightness at the start of event
*/

/*
LED array PWM initializing...

allocated: 11 total: 11

allocated[]: 48 total: 59

allocated: 15 total: 74

allocated[]: 200 total: 274

allocated[]: 32 total: 306

allocated: 23 total: 329

allocated: 23 total: 352

allocated: 13 total: 365

allocated: 13 total: 378

allocated: 23 total: 401

allocated: 13 total: 414

allocated: 13 total: 427

allocated: 23 total: 450

allocated: 23 total: 473

allocated: 23 total: 496

allocated: 23 total: 519

allocated: 13 total: 532

allocated: 13 total: 545

allocated: 13 total: 558

allocated: 14 total: 572
*/

#define LED_ARRAY_SIZE                   16
#define LED_CYCLE_DURATION_MICROS        ((unsigned long) 6000)
#define LED_MAX                          LED_CYCLE_DURATION_MICROS

char ledPins[] = { 2, 3, 4, 13, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1 };

static LedArray *ledArray;
static EventPlayer *player;

void run() {
  unsigned long time = micros();
  player->run(time);
}

void setup() {
  Serial.begin(115200);
  Serial.println("LED array PWM initializing...");

  ledArray = new LedArray(LED_ARRAY_SIZE, LED_CYCLE_DURATION_MICROS);
  LedArrayConfig::ledArray = ledArray;

  player = new EventPlayer(100, LED_ARRAY_SIZE);

  for (unsigned char i = 0; i < LED_ARRAY_SIZE; i++) {
    ledArray->setLedPin(i, ledPins[i]);
  }

  player->add(new FadeToLedBrightnessEvent(1000, 8999, 3, LED_MAX));
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

  player->add(new JumpEvent(17000, 0, -1));

  player->prepare();

  MsTimer2::set(5, run);
  MsTimer2::start();

  player->start(micros());
}

void loop() {
  unsigned long time = micros();

  ledArray->update(time);


  /*long endTime = micros();
  
  if (time % (unsigned long) 100000 == 0) {
    Serial.println(endTime - time, DEC);
    Serial.println();
  }*/
}

