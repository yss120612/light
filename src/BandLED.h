#ifndef _BANDLED_h
#define _BANDLED_h
#include <Arduino.h>
#include "Settings.h"
#include "Config.h"

class BandLED{
    public:
BandLED();
void setup();
void off();
void on();
void swc();
void setOne(uint8_t cannel, uint8_t value);
private:
void refresh();
};
#endif