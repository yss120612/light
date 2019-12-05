#ifndef _BANDLED_h
#define _BANDLED_h
#include <Arduino.h>
#include "Settings.h"
class BandLED{
    public:
BandLED();
void setup();
void off();
void on();
uint8_t getCW();
uint8_t getNW();
uint8_t getWW();
void setOne(uint8_t cannel, uint8_t value);
private:
void refresh();
uint8_t cw,nw,ww;
};
#endif