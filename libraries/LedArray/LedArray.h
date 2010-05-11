#ifndef LedArray_h
#define LedArray_h

#include "WProgram.h"

class LedState {
  public:
    LedState();
    LedState(char pin);
    char pin;
    unsigned short brightness;
    unsigned short storedBrightness;
};

class LedArray {
  public:
    LedArray(unsigned char ledCount, unsigned long cycleDurationMicros);
    ~LedArray();
    unsigned char getLedCount();
    void setLedPin(unsigned char index, char pin);
    void disableLed(unsigned char index);
    void setLedBrightness(unsigned char index, unsigned short brightness);
    void setRelativeLedBrightness(unsigned char index, short relativeBrightness);
    unsigned short getLedBrightness(unsigned char index);
    boolean hasLedMaxBrightness(unsigned char index);
    boolean hasLedMinBrightness(unsigned char index);
    void storeLedBrightness(unsigned char index);
    void setStoredLedBrightness(unsigned char index, unsigned short storedBrightness);
    unsigned short getStoredLedBrightness(unsigned char index);
    void update(unsigned long time);
  private:
    unsigned long cycleDurationMicros;
    unsigned long previousTime;
    unsigned char ledCount;
    LedState *ledState;
};

namespace LedArrayConfig {
	extern LedArray *ledArray;
}

#endif


