#include "Relay.h"


void Relay::setup(uint8_t tp)
{
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    //ison = !level;
    syncro();
    
    type = tp;
    armed=false;
    
}


void Relay::setState(bool s){
    ison=s;
    boolean
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
   setState(!ison);
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