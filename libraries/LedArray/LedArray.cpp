/*
 * LedArray.cpp - Library for controlling LED arrays and LED brightness.
 */
 
#include "WProgram.h"
#include "DynamicMemory.h"
#include "LedArray.h"

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
}

LedArray::~LedArray() {
	delete[] this->ledState;
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

void LedArray::setRelativeLedBrightness(unsigned char index, short relativeBrightness) {
  short newBrightness = (short) ledState[index].brightness + relativeBrightness;
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
	  if (state->brightness > 0) {
		digitalWrite(state->pin, HIGH);
	  } else {
		digitalWrite(state->pin, LOW);
	  }
      continue;
    }

    if (elapsed >= state->brightness) {
      digitalWrite(state->pin, LOW);
    }
  }
}