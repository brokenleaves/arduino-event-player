/*
 * LedArray.cpp - Library for controlling LED arrays and LED brightness.
 */
 
#include "WProgram.h"
#include "LedArray.h"

void *operator new(size_t size) {
  return malloc(size);
}

void operator delete(void *ptr) {
  free(ptr);
}

void *operator new[](size_t size) {
    return malloc(size);
}

void operator delete[](void *ptr) {
    free(ptr);
}

LedState::LedState() {
  this->pin = -1;
  this->brightness = 0;
}

LedState::LedState(char pin) {
  this->pin = pin;
  this->brightness = 0;
}

LedArray::LedArray(unsigned char ledCount, unsigned long cycleDurationMicros) {
  this->ledCount = ledCount;
  this->cycleDurationMicros = cycleDurationMicros;
  this->previousTime = 0;
  this->ledState = new LedState[ledCount];
  this->counter = 0;
}

void LedArray::setLedPin(unsigned char index, char pin) {
  pinMode(pin, OUTPUT);
  ledState[index].pin = pin;
  ledState[index].brightness = 0;
}

void LedArray::disableLed(unsigned char index) {
  // Change mode to input?
  pinMode(ledState[index].pin, OUTPUT);
  digitalWrite(ledState[index].pin, LOW);

  ledState[index].pin = -1;
  ledState[index].brightness = 0;
}

void LedArray::setLedBrightness(unsigned char index, unsigned short brightness) {
  if (brightness > cycleDurationMicros) {
    brightness = cycleDurationMicros;
  }
  ledState[index].brightness = brightness;
}

void LedArray::increaseLedBrightness(unsigned char index, short brightness) {
  short newBrightness = (short) ledState[index].brightness + brightness;
  if (newBrightness > (short) cycleDurationMicros) {
    newBrightness = (short) cycleDurationMicros;
  }
  if (newBrightness < 0) {
    newBrightness = 0;
  }
  ledState[index].brightness = newBrightness;
}

unsigned short LedArray::getLedBrightness(unsigned char index) {
  return ledState[index].brightness;
}

boolean LedArray::hasLedMaxBrightness(unsigned char index) {
  return (ledState[index].brightness >= cycleDurationMicros);
}

boolean LedArray::hasLedMinBrightness(unsigned char index) {
  return (ledState[index].brightness <= 0);
}

void LedArray::update(unsigned long time) {
  unsigned long elapsed = time - previousTime;
  boolean cycleBegins = (elapsed >= cycleDurationMicros);
  
  if (cycleBegins) {
    previousTime = previousTime + cycleDurationMicros;
  }

  for (unsigned char i = 0; i < ledCount; i++) {
    LedState *state = &ledState[i];

    if (state->pin < 0) {
      continue;
    }

    if (cycleBegins) {
      digitalWrite(state->pin, HIGH);
      continue;
    }

    if (elapsed >= state->brightness) {
      digitalWrite(state->pin, LOW);
    }
  }
}