#ifndef _RELAYS_h
#define _RELAYS_h
#include <Arduino.h>
#include "Settings.h"


class Relay{
    public:
    Relay();
    void setup(uint8_t p, uint8_t tp=RELTYPE_SWICH,bool lvl=false);
    bool isOn();
    void setOn();
    void setOff();
    bool swc();
    void arm();
    void disarm();
    bool ison;
    void setState(boolean s);
    protected :
    uint8_t type;
    
    uint8_t pin;
    void syncro();
    bool level;
    bool armed;
};


#endif