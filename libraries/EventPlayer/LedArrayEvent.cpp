/*
 * LedArrayEvent.cpp
 */
 
#include "WProgram.h"
#include "LedArrayEvent.h"

#define calculateBrightness(fromBrightness, toBrightness, eventTimeElapsed, lengthMillis) \
	(((fromBrightness) < (toBrightness)) ?				\
		(((fromBrightness) + ((((eventTimeElapsed) * 1000) / (lengthMillis)) * ((toBrightness) - (fromBrightness))) / 1000)) : \
		(((fromBrightness) - ((((eventTimeElapsed) * 1000) / (lengthMillis)) * ((fromBrightness) - (toBrightness))) / 1000)))

void SetLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	LedArrayConfig::ledArray->setLedBrightness(ledIndex, brightness);
}

void SetRelativeLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	LedArrayConfig::ledArray->setRelativeLedBrightness(ledIndex, relativeBrightness);
}

void FadeLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	LedArrayConfig::ledArray->setLedBrightness(ledIndex,
		calculateBrightness(fromBrightness, toBrightness, playerTimeMillis - timeMillis, lengthMillis));
}

void FadeToLedBrightnessEvent::init(unsigned long playerTimeMillis) {
	LedArrayConfig::ledArray->storeLedBrightness(ledIndex);
}

void FadeToLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	unsigned long fromBrightness = LedArrayConfig::ledArray->getStoredLedBrightness(ledIndex);
	LedArrayConfig::ledArray->setLedBrightness(ledIndex,
		calculateBrightness(fromBrightness, toBrightness, playerTimeMillis - timeMillis, lengthMillis));
}

void FadeRelativeLedBrightnessEvent::init(unsigned long playerTimeMillis) {
	LedArrayConfig::ledArray->storeLedBrightness(ledIndex);
}

void FadeRelativeLedBrightnessEvent::run(unsigned long playerTimeMillis) {
	unsigned long fromBrightness = LedArrayConfig::ledArray->getStoredLedBrightness(ledIndex);
	unsigned long toBrightness = fromBrightness + toRelativeBrightness;
	LedArrayConfig::ledArray->setLedBrightness(ledIndex,
		calculateBrightness(fromBrightness, toBrightness, playerTimeMillis - timeMillis, lengthMillis));
}
