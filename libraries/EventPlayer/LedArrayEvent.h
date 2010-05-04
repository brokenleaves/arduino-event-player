#ifndef LedArrayEvent_h
#define LedArrayEvent_h

#include "WProgram.h"

#include "EventPlayer.h"
#include "LedArray.h"

class SetLedBrightnessEvent : public OnetimeEvent {
	public:
		SetLedBrightnessEvent(unsigned long timeMillis, unsigned char ledIndex, unsigned short brightness) : OnetimeEvent(timeMillis) {
			this->ledIndex = ledIndex;
			this->brightness = brightness;
		}
		virtual void run(unsigned long playerTimeMillis);
	private:
		unsigned char ledIndex;
		unsigned short brightness;
};

class SetRelativeLedBrightnessEvent : public OnetimeEvent {
	public:
		SetRelativeLedBrightnessEvent(unsigned long timeMillis, unsigned char ledIndex, short relativeBrightness) : OnetimeEvent(timeMillis) {
			this->ledIndex = ledIndex;
			this->relativeBrightness = relativeBrightness;
		}
		virtual void run(unsigned long playerTimeMillis);
	private:
		unsigned char ledIndex;
		short relativeBrightness;
};

class FadeLedBrightnessEvent : public ContinuousEvent {
	public:
		FadeLedBrightnessEvent(unsigned long timeMillis, unsigned long lengthMillis, unsigned char ledIndex, unsigned short fromBrightness, unsigned short toBrightness) : ContinuousEvent(timeMillis, lengthMillis) {
			this->ledIndex = ledIndex;
			this->fromBrightness = fromBrightness;
			this->toBrightness = toBrightness;
		}
		virtual void run(unsigned long playerTimeMillis);
	private:
		unsigned char ledIndex;
		unsigned short fromBrightness;
		unsigned short toBrightness;
};

class FadeToLedBrightnessEvent : public ContinuousEvent {
	public:
		FadeToLedBrightnessEvent(unsigned long timeMillis, unsigned long lengthMillis, unsigned char ledIndex, unsigned short toBrightness) : ContinuousEvent(timeMillis, lengthMillis) {
			this->ledIndex = ledIndex;
			this->toBrightness = toBrightness;
		}
		virtual void init(unsigned long playerTimeMillis);
		virtual void run(unsigned long playerTimeMillis);
	private:
		unsigned char ledIndex;
		unsigned short toBrightness;
};

class FadeRelativeLedBrightnessEvent : public ContinuousEvent {
	public:
		FadeRelativeLedBrightnessEvent(unsigned long timeMillis, unsigned long lengthMillis, unsigned char ledIndex, short toRelativeBrightness) : ContinuousEvent(timeMillis, lengthMillis) {
			this->ledIndex = ledIndex;
			this->toRelativeBrightness = toRelativeBrightness;
		}
		virtual void init(unsigned long playerTimeMillis);
		virtual void run(unsigned long playerTimeMillis);
	private:
		unsigned char ledIndex;
		short toRelativeBrightness;
};

#endif
