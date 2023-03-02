#include "Relay.h"


void Relay::setup(uint8_t p,rel_t tp,bool lvl)
{
    pin=p;
    state.level=lvl;
    //pinMode(pin, OUTPUT_OPEN_DRAIN);
    pinMode(pin, OUTPUT);
    
    state.type = tp;
    state.armed=false;
}


void Relay::setState(bool s){
    state.ison=s;
    syncro();
}

bool Relay::isOn()
{   
    return state.ison;
}

void Relay::setOn()
{
   //if (!state.ison)
   // {
        setState(true);
   // }
}

void Relay::setOff()
{
   //if (state.ison)
   // {
        setState(false);
   // }
}

void Relay::syncro(){
    //Serial.print("relay set ");
    //Serial.println(state.ison);
    digitalWrite(pin, state.ison?state.level:!state.level);
}



bool Relay::swc()
{
 if (state.ison) setState(false);
      else setState(true);
   return state.ison;
}

void Relay::arm()
{
    if (state.type == RELTYPE_BUTTON){
        state.armed=true;   
        setOn();
    }
}

bool Relay::isButton(){
    return state.type == RELTYPE_BUTTON;
}

void Relay::disarm()
{
    if (state.type == RELTYPE_BUTTON){
        state.armed=false;   
        setOff();
    }
}