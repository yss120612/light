#ifndef _RELAYS_h
#define _RELAYS_h
#include <Arduino.h>

class Relay{
    public:
    Relay(uint8_t p);
    void Setup();
    boolean isOn();
    void setOn();
    void setOff();
    void set(boolean w);
    boolean swc();
    void loop(unsigned long t);
    void arm(unsigned long t);
    private:
    boolean state;
    uint8_t pin;
    unsigned long tm;
};


#endif