#include "Relay.h"


void Relay::setup(uint8_t tp)
{
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    type = tp;
    armed=false;
}


void Relay::setState(boolean s){
    state=s;
    syncro();
}

boolean Relay::isOn()
{
    return state;
}

void Relay::setOn()
{
    if (!state)
    {
        setState(true);
    }
}

void Relay::setOff()
{
    if (state)
    {
        setState(false);
    }
}

void Relay::syncro(){
    digitalWrite(pin, state?level:!level);
}



boolean Relay::swc()
{
   setState(!state);
   return state;
}

bool Relay::arm()
{
    if (type == RELTYPE_BUTTON){
        armed=true;   
        setOn();
        return true;
    }
    return false;
}


void Relay::disarm()
{
    if (type == RELTYPE_BUTTON){
        armed=false;   
        setOff();
    }
}