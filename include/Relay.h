#ifndef _RELAYS_h
#define _RELAYS_h
#include <Arduino.h>
#include "Settings.h"


class Relay{
    public:
    Relay(uint8_t p,bool lvl=false){pin=p,level=lvl;};
    void setup(boolean * mst=NULL,uint8_t tp=RELTYPE_SWICH);
    boolean isOn();
    void setOn();
    void setOff();
    void set(boolean w);
    boolean swc();
    void arm(unsigned long t);
    uint8_t type;
    protected :
    boolean  state;
    //boolean  * mem_state;
    uint8_t pin;
    unsigned long tm;
    uint16_t dur;
    void setState(boolean s);
    void syncro();
    bool level;
};


#endif