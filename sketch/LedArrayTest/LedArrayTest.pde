#include "DynamicMemory.h"
#include "LedArray.h"
#include "EventPlayer.h"
#include "LedArrayEvent.h"

#define LED_ARRAY_SIZE                   4
#define LED_CYCLE_DURATION_MICROS        ((unsigned long) 4000)

char ledPins[] = { 2, 3, 4, 13, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1 };

unsigned long time = 0;

LedArray ledArray(LED_ARRAY_SIZE, LED_CYCLE_DURATION_MICROS);
EventPlayer player(100, LED_ARRAY_SIZE);

void setup() {
  Serial.begin(115200);
  Serial.println("LED array PWM initializing...");

  for (unsigned char i = 0; i < LED_ARRAY_SIZE; i++) {
    ledArray.setLedPin(i, ledPins[i]);
  }

  player.add(new FadeLedBrightnessEvent(1000, 4000, &ledArray, 0, 0, 4000));
  player.add(new SetLedBrightnessEvent(2000, &ledArray, 1, 4000));
  player.add(new SetLedBrightnessEvent(3000, &ledArray, 2, 4000));

  player.add(new FadeLedBrightnessEvent(5000, 2000, &ledArray, 1, 1000, 0));
  player.add(new SetLedBrightnessEvent(6000, &ledArray, 0, 0));
  player.add(new SetRelativeLedBrightnessEvent(7000, &ledArray, 2, -3500));

  player.add(new FadeLedBrightnessEvent(10000, 4000, &ledArray, 0, 0, 4000));
  player.add(new FadeLedBrightnessEvent(10000, 4000, &ledArray, 1, 0, 4000));
  player.add(new FadeLedBrightnessEvent(10000, 4000, &ledArray, 2, 0, 4000));

  player.add(new SetLedBrightnessEvent(15000, &ledArray, 0, 0));
  player.add(new SetLedBrightnessEvent(15000, &ledArray, 1, 0));
  player.add(new SetLedBrightnessEvent(15000, &ledArray, 2, 0));

  player.add(new JumpEvent(17000, 0, -1));
  /*player.add(new SetRelativeLedBrightnessEvent(4000, &ledArray, 1, 1000));
  player.add(new FadeLedBrightnessEvent(5000, 3000, &ledArray, 2, 0, 2048));
  player.add(new FadeLedBrightnessEvent(10000, 3000, &ledArray, 2, 2048, 0));*/
  player.prepare();
  player.start(micros());
}

void loop() {
  time = micros();

  player.run(time);
  ledArray.update(time);

  /*long endTime = micros();
  
  if (time % (unsigned long) 100000 == 0) {
    Serial.println(endTime - time, DEC);
    Serial.println();
  }*/
}

