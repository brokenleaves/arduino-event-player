#include "LedArray.h"

#define DURATION_CYCLE        ((unsigned long) 2048)

#define DURATION_CHANGE_CYCLE  ((unsigned long) 2 * (unsigned long) 1000)

#define LED_ARRAY_SIZE        3

char ledPins[] = { 13, 3, 2, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1 };
char ledDeltas[] = { 2, 3, 5, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0 };

unsigned long previousChangeTime = 0;
char delta[LED_ARRAY_SIZE];

unsigned long time = 0;

LedArray ledArray(LED_ARRAY_SIZE, DURATION_CYCLE);

void setup() {
  Serial.begin(115200);
  Serial.println("LED array PWM initializing...");
  for (unsigned char i = 0; i < LED_ARRAY_SIZE; i++) {
    ledArray.setLedPin(i, ledPins[i]);
    delta[i] = ledDeltas[i];
  }
}

void loop() {
  time = micros();

  unsigned long changeTimeDiff = time - previousChangeTime;
  if (changeTimeDiff >= DURATION_CHANGE_CYCLE) {
    previousChangeTime = previousChangeTime + DURATION_CHANGE_CYCLE;

    for (unsigned char i = 0; i < LED_ARRAY_SIZE; i++) {
      if (ledArray.hasLedMaxBrightness(i)) {
        delta[i] = -delta[i];
      }
  
      ledArray.increaseLedBrightness(i, delta[i]);
  
      if (ledArray.hasLedMinBrightness(i)) {
        delta[i] = -delta[i];
      }
    }
  }

  
  ledArray.update(time);

  /*long endTime = micros();
  
  if (time % (unsigned long) 100000 == 0) {
    Serial.println(endTime - time, DEC);
    Serial.println();
  }*/
}

