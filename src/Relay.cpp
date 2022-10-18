#include "Relay.h"


void Relay::setup(boolean * mst,uint8_t tp)
{
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    state = !level;
    syncro();
    
    type = tp;
    armed=false;
    
}


void Relay::setState(boolean s){
    state=s?level:!level;
    
    // if (mem_state!=NULL){
    //     *mem_state=s;
    //     conf.force_story();
    // }
}

boolean Relay::isOn()
{
    return state==level;
}

void Relay::setOn()
{
    if (!state)
    {
        setState(true);
        syncro();
    }
}

void Relay::setOff()
{
    if (state)
    {
        setState(false);
        syncro();
    }
}

void Relay::syncro(){
    digitalWrite(pin, state?level:!level);
}

void Relay::set(boolean w)
{
    if (w)
        setOn();
    else
        setOff();
}

boolean Relay::swc()
{
   set(!state);
   return state;
}

void Relay::arm()
{
    if (type == RELTYPE_BUTTON){
        armed=true;   
        setOn();
    }
}


void Relay::disarm()
{
    if (type == RELTYPE_BUTTON){
        armed=false;   
        setOff();
    }
}