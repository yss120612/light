#ifndef _RELAYS_h
#define _RELAYS_h
#include <Arduino.h>
#include "Settings.h"
#include "Config.h"


class Relay{
    public:
    Relay(uint8_t p);
    void setup(boolean * mst=NULL,uint8_t tp=RELTYPE_SWICH);
    boolean isOn();
    void setOn();
    void setOff();
    void set(boolean w);
    boolean swc();
    void loop(unsigned long t);
    void arm(unsigned long t);
    uint8_t type;
    private:
    boolean  state;
    boolean  * mem_state;
    uint8_t pin;
    unsigned long tm;
    uint16_t dur;
    protected :
    void setState(boolean s);
    void syncro();
};


#endif