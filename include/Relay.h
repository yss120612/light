#ifndef _RELAYS_h
#define _RELAYS_h
#include <Arduino.h>
#include "Settings.h"


class Relay{
    public:
    Relay(uint8_t p,bool lvl=false){pin=p,level=lvl;};
    void setup(uint8_t tp=RELTYPE_SWICH);
    boolean isOn();
    void setOn();
    void setOff();
    boolean swc();
    bool arm();
    void disarm();
    uint8_t type;
    void setState(boolean s);
    protected :
    boolean  state;
    //boolean  * mem_state;
    uint8_t pin;
    void syncro();
    bool level;
    bool armed;
};


#endif