#include "Relay.h"


void Relay::setup(uint8_t p,uint8_t tp,bool lvl)
{
    pin=p;
    level=lvl;
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    syncro();
    type = tp;
    armed=false;
    
}


void Relay::setState(bool s){
    ison=s?true:false;
    Serial.print(" sss=");
    Serial.print(s);
    Serial.print(" ISON=");
    Serial.println(ison);
    syncro();
}

bool Relay::isOn()
{   
    return ison;
}

void Relay::setOn()
{
    if (!ison)
    {
        setState(true);
    }
}

void Relay::setOff()
{
    if (ison)
    {
        setState(false);
    }
}

void Relay::syncro(){
    digitalWrite(pin, ison?level:!level);
    Serial.print("pin=");
    Serial.print(pin);
    Serial.print(" ison=");
    Serial.println(ison);
}



bool Relay::swc()
{
   setState(!ison?true:false);
   return ison;
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