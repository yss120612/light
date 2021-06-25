#include "Relays.h"
#include "Log.h"

Relay::Relay(uint8_t p)
{
    pin = p;
    tm = 0;
}

void Relay::setup(boolean * mst,uint8_t tp)
{
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    //digitalWrite(pin, HIGH);
    state = false;
    if (mst!=NULL){
        mem_state=mst;
        state=*mst;
        //logg.logging("mem="+String((int)mem_state) +"val="+String(*mem_state));
    }
    
    syncro();
    //logg.logging("mem2="+String((int)mem_state));
    type = tp;
    if (type == RELTYPE_BUTTON){
        dur = PRESS_DURATION;
    }
    else{
        dur=0;
    }
}


void Relay::setState(boolean s){
    state=s;
    
    if (mem_state!=NULL){
        *mem_state=s;
        
        conf.force_story();
    }
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
    digitalWrite(pin, state?LOW:HIGH);
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

void Relay::arm(unsigned long t)
{
    tm = t;
    setOn();
}

void Relay::loop(unsigned long t)
{
    if (tm > 0 && dur > 0 && t - tm > dur)
    {
        tm = 0;
        setOff();
    }
}
