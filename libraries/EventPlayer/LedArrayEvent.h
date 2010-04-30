#ifndef LedArrayEvent_h
#define LedArrayEvent_h

#include "WProgram.h"

#include "EventPlayer.h"
#include "LedArray.h"

class SetLedBrightnessEvent : public OnetimeEvent {
	public:
		SetLedBrightnessEvent(unsigned long timeMillis, LedArray *ledArray, unsigned char ledIndex, unsigned short brightness) : OnetimeEvent(timeMillis) {
			this->ledArray = ledArray;
			this->ledIndex = ledIndex;
			this->brightness = brightness;
		}
		virtual void run(unsigned long playerTimeMillis);
	private:
		LedArray *ledArray;
		unsigned char ledIndex;
		unsigned short brightness;
};

class SetRelativeLedBrightnessEvent : public OnetimeEvent {
	public:
		SetRelativeLedBrightnessEvent(unsigned long timeMillis, LedArray *ledArray, unsigned char ledIndex, short relativeBrightness) : OnetimeEvent(timeMillis) {
			this->ledArray = ledArray;
			this->ledIndex = ledIndex;
			this->relativeBrightness = relativeBrightness;
		}
		virtual void run(unsigned long playerTimeMillis);
	private:
		LedArray *ledArray;
		unsigned char ledIndex;
		short relativeBrightness;
};

class FadeLedBrightnessEvent : public ContinuousEvent {
	public:
		FadeLedBrightnessEvent(unsigned long timeMillis, unsigned long lengthMillis, LedArray *ledArray, unsigned char ledIndex, unsigned short fromBrightness, unsigned short toBrightness) : ContinuousEvent(timeMillis, lengthMillis) {
			this->ledArray = ledArray;
			this->ledIndex = ledIndex;
			this->fromBrightness = fromBrightness;
			this->toBrightness = toBrightness;
		}
		virtual void run(unsigned long playerTimeMillis);
	private:
		LedArray *ledArray;
		unsigned char ledIndex;
		unsigned short fromBrightness;
		unsigned short toBrightness;
};

#endif
