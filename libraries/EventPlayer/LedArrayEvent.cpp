/*
 * LedArrayEvent.cpp
 */
 
#include "WProgram.h"
#include "LedArrayEvent.h"

void SetLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	ledArray->setLedBrightness(ledIndex, brightness);
}

void SetRelativeLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	ledArray->setRelativeLedBrightness(ledIndex, relativeBrightness);
}

void FadeLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	unsigned long eventTimeElapsed = playerTimeMillis - timeMillis;
	unsigned long brightnessDiff;
	unsigned long brightness;

	if (fromBrightness < toBrightness) {
		brightnessDiff = toBrightness - fromBrightness;
		brightness = fromBrightness + (((eventTimeElapsed * 1000) / lengthMillis) * brightnessDiff) / 1000;
	} else {
		brightnessDiff = fromBrightness - toBrightness;
		brightness = fromBrightness - (((eventTimeElapsed * 1000) / lengthMillis) * brightnessDiff) / 1000;
	}

	ledArray->setLedBrightness(ledIndex, brightness);
}
