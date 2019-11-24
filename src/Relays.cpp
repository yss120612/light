#include "Relays.h"

Relay::Relay(uint8_t p){
    pin=p;
    tm=0;
}

void Relay::Setup(){
    pinMode(pin,OUTPUT_OPEN_DRAIN);
    digitalWrite(pin,HIGH);
    state=false;
}

boolean Relay::isOn(){
    return state;
}

void Relay::setOn(){
    if (!state){
        digitalWrite(pin,LOW);
        state=true;
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
    //if (tm>0)
}

void Relay::loop(unsigned long t){
    if (tm>0 && t>tm){
        tm=0;
        setOff();
    }
}
    