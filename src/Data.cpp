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
    btns.setEvents(&evts);
    btns.add(1,HIGH);
    ir.enable();
}

void AppData::loop(unsigned long t)
{
    if (ir.checkIR(t)>0){
        evts.putPultEvent((uint8_t)ir.getCommand());
    }

    ProcessEvents(t);  

    for (uint8_t i = 0; i < lgh; i++)
        relays[i].loop(t);
}


void AppData::ir_sleep(){
    ir.sleep_sometime();
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

void AppData::ProcessEvents(unsigned long t){
event_t ev;
if (evts.getEvent(&ev,t)){
  switch (ev.state)
  {
  case BTN_CLICK:
    //logg.logging("CLICK "+ String(ev.button)+" count="+String(ev.count)+" wait="+String(t-ev.wait_time)+ " millis="+String(t));
    if (ev.count==1)  
    {
      //logg.logging(rtc.timestring());
    }else{
      
    }
    if (ev.count==2) {
      //logg.logging("Success= "+ String(rtc.isSuccess()));
      //+" time="+rtc.timestring());
      //logg.logging(rtc.test());
      // pinMode(D0,OUTPUT);
      // pinMode(D1,OUTPUT);
      // pinMode(D2,OUTPUT);
      // digitalWrite(D0,HIGH);
      // digitalWrite(D1,HIGH);
      // digitalWrite(D2,HIGH);
    }
    break;
  case BTN_LONGCLICK:
    logg.logging("LONGCLICK "+ String(ev.button)+" count="+String(ev.count));
    break;
  case PULT_BUTTON:
  switch (ev.button){
    case PULT_1:
      relaySwitch(0, t);
      break;
    case PULT_2:
      relaySwitch(1, t);
      break;
    case PULT_3:
      relaySwitch(2, t);
      swcLight(isOn(2));
      break;
    case PULT_4:
      relaySwitch(3, t);
      break;
    case PULT_POWER:
      relayOff(t);
      break;
    case PULT_SOUND:
      relaySwitchOff(t);
      break;
    case PULT_VOLDOWN:
      tuneLight(false,CANNEL_CW);
      break;
    case PULT_VOLUP:
      tuneLight(true,CANNEL_CW);
      break;
    case PULT_FASTBACK:
      tuneLight(false,CANNEL_NW);
      break;
    case PULT_FASTFORWARD:
      tuneLight(true,CANNEL_NW);
      break;
    case PULT_PREV:
      tuneLight(false,CANNEL_WW);
      break;
    case PULT_NEXT:
      tuneLight(true,CANNEL_WW);
      break;
    case PULT_SLOW://ultra low
      setOneBand(CANNEL_CW,0);
      setOneBand(CANNEL_NW,0);
      setOneBand(CANNEL_WW,64);
      break;
    case PULT_ZOOM://low
      setOneBand(CANNEL_CW,64);
      setOneBand(CANNEL_NW,64);
      setOneBand(CANNEL_WW,64);
      break;
    case PULT_STOP://middle
      setOneBand(CANNEL_CW,128);
      setOneBand(CANNEL_NW,128);
      setOneBand(CANNEL_WW,128);
      break;
    case PULT_PAUSE://full
      setOneBand(CANNEL_CW,255);
      setOneBand(CANNEL_NW,255);
      setOneBand(CANNEL_WW,255);
      break;
    
  }
    break;

    case WEB_EVENT:
        switch (ev.button){
    case PULT_1:
      relaySet(0,ev.count>0);
      break;
    case PULT_2:
      relaySet(1,ev.count>0);
      break;
    case PULT_3:
      relaySet(2,ev.count>0);
      swcLight(ev.count>0);
      break;
    case PULT_4:
      relaySwitch(3, t);
      break;
    case CANNEL_CW:
      setOneBand(CANNEL_CW,ev.count);
      break;
    case CANNEL_NW:
      setOneBand(CANNEL_NW,ev.count);
      break;
    case CANNEL_WW:
      setOneBand(CANNEL_WW,ev.count);
      break;
      }
    
    break;
  }
}
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