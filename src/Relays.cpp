#include "Relays.h"

Relay::Relay(uint8_t p){
    pin=p;
    tm=0;
}

void Relay::setup(uint8_t tp){
    pinMode(pin,OUTPUT_OPEN_DRAIN);
    digitalWrite(pin,HIGH);
    state=false;
    type=tp;
    if (type==RELTYPE_SWICH) dur=PRESS_DURATION;
}

boolean Relay::isOn(){
    return state;
}

void Relay::setOn(){
    if (!state){
        digitalWrite(pin,LOW);
        state=true;
        dur=0;
    }
}
    void Relay::setOff(){
        if (state){
        digitalWrite(pin,HIGH);
        state=false;
    }
    }
    void Relay::set(boolean w){
        if (w) setOn();
        else setOff();
    }

    boolean Relay::swc(){
        set(!state);
    }

void Relay::arm(unsigned long t){
    tm=t;
    setOn();
}

void Relay::loop(unsigned long t){
    if (tm>0 && dur>0 && t-tm>dur){
        tm=0;
        setOff();
    }
}
    