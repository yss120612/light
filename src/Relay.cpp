#include "Relay.h"


void Relay::setup(uint8_t p,uint8_t tp,bool lvl)
{
    pin=p;
    level=lvl;
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    //syncro();
    type = tp;
    armed=false;
}


void Relay::setState(bool s){
    ison=(s?true:false);
    uint8_t iso=(s?1:0);
    iso=1;
    Serial.print("s=");
    Serial.print(s);
    Serial.print(" iso=");
    Serial.print(iso);
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
}



bool Relay::swc()
{
   if (ison) setState(false);
   else setState(true);
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