#include "Data.h"
#include "Log.h"


AppData::AppData()
{
}

void AppData::setup()
{
    conf.load();
    relays[0].setup();
    relays[1].setup();
    relays[2].setup();
    relays[3].setup(RELTYPE_BUTTON);
    lamp.setup();
}


uint8_t AppData::getCW(){return conf.cw;}
    uint8_t AppData::getNW(){return conf.nw;}
    uint8_t AppData::getWW(){return conf.ww;}


void AppData::relaySwitchOff(unsigned long t)
{
    for (uint8_t i = 0; i < lgh; i++)
    {
        switch (relays[i].type)
        {
        case RELTYPE_SWICH:
            relays[i].setOff();
            break;
        case RELTYPE_BUTTON:
            relays[i].arm(t);
            break;
        }
    }
}

void AppData::relayOff(unsigned long t)
{
    for (uint8_t i = 0; i < lgh; i++)
    {
        switch (relays[i].type)
        {
        case RELTYPE_SWICH:
            relays[i].setOff();
            break;
        }
    }
}

void AppData::relaySet(uint8_t i, boolean st)
{
    switch (relays[i].type)
    {
    case RELTYPE_SWICH:
        relays[i].set(st);
        break;
    }
}

void AppData::relaySwitch(uint8_t i, unsigned long t)
{
    switch (relays[i].type)
    {
    case RELTYPE_SWICH:
        relays[i].swc();
        break;
    case RELTYPE_BUTTON:
        relays[i].arm(t);
        break;
    }
}

boolean AppData::isOn(uint8_t i)
{
    return relays[i].isOn();
}

boolean AppData::isOn()
{
    boolean ison = false;
    for (uint8_t i = 0; i < lgh; i++)
    if (relays[i].type==RELTYPE_SWICH)
        ison |= relays[i].isOn();
    return ison;
}

void AppData::loop(unsigned long t)
{
    for (uint8_t i = 0; i < lgh; i++)
        relays[i].loop(t);
}

void AppData::setOneBand(uint8_t cannel, uint8_t val){
    //if (val>0) lamp.on();
    lamp.setOne(cannel,val);
}

void AppData::swcLight(boolean state){
    if (state) lamp.on();
    else lamp.off();
}

void AppData::tuneLight(boolean dir,uint8_t cannel){
  if (dir) lamp.colorUp(cannel);
  else lamp.colorDown(cannel);
}

void AppData::logConf(){
    conf.print();
}