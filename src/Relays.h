#ifndef _RELAYS_h
#define _RELAYS_h
#include <Arduino.h>
#include "Settings.h"



class Relay{
    public:
    Relay(uint8_t p);
    void setup(uint8_t tp=RELTYPE_SWICH);
    boolean isOn();
    void setOn();
    void setOff();
    void set(boolean w);
    boolean swc();
    void loop(unsigned long t);
    void arm(unsigned long t);
    uint8_t type;
    private:
    boolean state;
    uint8_t pin;
    unsigned long tm;
    uint16_t dur;
};


#endif