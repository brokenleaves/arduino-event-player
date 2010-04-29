#ifndef LedArray_h
#define LedArray_h

#include "WProgram.h"

class LedState {
  public:
    LedState();
    LedState(char pin);
    char pin;
    unsigned short brightness;
};

class LedArray {
  public:
    LedArray(unsigned char ledCount, unsigned long cycleDurationMicros);
    void setLedPin(unsigned char index, char pin);
    void disableLed(unsigned char index);
    void setLedBrightness(unsigned char index, unsigned short brightness);
    void increaseLedBrightness(unsigned char index, short brightness);
    unsigned short getLedBrightness(unsigned char index);
	boolean hasLedMaxBrightness(unsigned char index);
	boolean hasLedMinBrightness(unsigned char index);
    void update(unsigned long time);
  private:
    unsigned long cycleDurationMicros;
    unsigned long previousTime;
    unsigned char ledCount;
    LedState *ledState;
};

#endif


