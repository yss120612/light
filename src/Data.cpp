#include "Data.h"
#include "Log.h"
#include "Settings.h"

AppData::AppData()
{
}

void AppData::setup(MqttClient * mq)
{
  if (!conf.setup())
  {
    logg.logging("AT24C32 ERROR!");
  }
  else{
    logg.logging("AT24C32 OK!");
  }
  conf.upload();
  conf.print();
  relays[0].setup(&conf.notebook_on);
  relays[1].setup();
  relays[2].setup(&conf.lamp_on);
  relays[3].setup(NULL,RELTYPE_BUTTON);
  //logg.logging("lamp="+String((int)&conf.lamp_on)+" val="+String(conf.lamp_on));
  //logg.logging("NB="+String((int)&conf.notebook_on)+" val="+String(conf.notebook_on));

  lamp.setup();

  // conf.notebook_on=false;
  // conf.heater_on=true;
  // conf.lamp_on=false;
  btns.setEvents(&evts);
  if (btns.add(BUTTON_1, HIGH)==0xFF)
  {
    logg.logging("error add button");
  }
  
  ir.enable();
  
  // lamp.force_refresh();
  // relays[2].set(conf.notebook_on);
  // if (conf.lamp_on)
  //   evts.putPultEvent((uint8_t)IR_DEVICE,PULT_3);
    
  display.setup();  
  fast_time_interval = true;
  last_tsync = 0;
  learn_commang=0;
  mqtt=mq;

}

void AppData::loop(unsigned long t)
{
  if (ir.checkIR(t) > 0)
  {
    evts.putPultEvent((uint8_t)ir.getDevice(), (uint8_t)ir.getCommand());
  }

  ProcessEvents(t);

  if (learn_commang>0 && t-learn_commang>60000)//10 min
  {
    learn_commang=0;
    display.showString("Learn mode","OFF","");
  }
  if (t < last_tsync) last_tsync=t;
  if (last_tsync==0 || t - last_tsync > (fast_time_interval ? SHORT_TIME : LONG_TIME))
  {
    last_tsync = t;
    update_time_from_inet();
  }

  for (uint8_t i = 0; i < lgh; i++)
    relays[i].loop(t);

  display.loop(t);  
}





void AppData::ir_sleep()
{
  ir.sleep_sometime();
}

uint8_t AppData::getCW() { return conf.cw; }
uint8_t AppData::getNW() { return conf.nw; }
uint8_t AppData::getWW() { return conf.ww; }


void AppData::relaySwitchOff(unsigned long t)
{
  relayOff();
  for (uint8_t i = 0; i < lgh; i++)
  {
    if (relays[i].type==RELTYPE_BUTTON) relays[i].arm(t);
  }
}

void AppData::relayOff()
{
  for (uint8_t i = 0; i < lgh; i++)
  {
    if (relays[i].type==RELTYPE_SWICH)
    {
      relays[i].setOff();
      if (mqtt) mqtt->updateState(i+1,relays[i].isOn());
    }
  }
}

void AppData::relaySet(uint8_t i, boolean st)
{
  switch (relays[i].type)
  {
  case RELTYPE_SWICH:
    relays[i].set(st);
    if (mqtt) mqtt->updateState(i+1,relays[i].isOn());
    break;
  }
}

void AppData::relaySwitch(uint8_t i, unsigned long t)
{
  switch (relays[i].type)
  {
  case RELTYPE_SWICH:
    relays[i].swc();
    if (mqtt) mqtt->updateState(i+1,relays[i].isOn());
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
    if (relays[i].type == RELTYPE_SWICH)
      ison |= relays[i].isOn();
  return ison;
}

void AppData::ProcessEvents(unsigned long t)
{
  event_t ev;
  if (evts.getEvent(&ev, t))
  {
    switch (ev.state)
    {
    case BTN_CLICK:
      //logg.logging("CLICK "+ String(ev.button)+" count="+String(ev.count)+" wait="+String(t-ev.wait_time)+ " millis="+String(t));
      if (ev.count == 1)
      {
        display.showString(rtc.now().timestamp(DateTime::TIMESTAMP_DATE), rtc.now().timestamp(DateTime::TIMESTAMP_TIME),String(rtc.getTemperature())+"C");
        //logg.logging(rtc.now().timestamp());
        //logg.logging(rtc.timestring());
      }
      else if (ev.count == 2)
      {
        learn_commang=t;
        display.showString("Learn mode","ON","");
        //logg.logging("Time is= "+ rtc.now().timestamp());
        //+" time="+rtc.timestring());
        //logg.logging(rtc.test());
        // pinMode(D0,OUTPUT);
        // pinMode(D1,OUTPUT);
        // pinMode(D2,OUTPUT);
        // digitalWrite(D0,HIGH);
        // digitalWrite(D1,HIGH);
        // digitalWrite(D2,HIGH);
      }
      else if (ev.count == 3){
        int error;
        for (int address = 1; address < 127; address++ )  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)    {
      logg.logging("I2C device found at address "+String(address));
      
    }
    //else if (error == 4)    {
      //Serial.print("Unknow error at address "+String(address));
    //}
  }
      }
      break;
    case BTN_LONGCLICK:
      if (ev.count==3) ESP.restart();
      break;
    case PULT_BUTTON:
      if (learn_commang>0){
        display.showString("Dev."+String(ev.count)+(ev.count==IR_DEVICE?" MY":" ALIEN"),"Code "+String(ev.button),"pult`");
      }
      if (ev.count!=IR_DEVICE) break;
      switch (ev.button)
      {
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
      
      
      case PULT_INFO:
        conf.print();
        break;
      case PULT_POWER:
        relayOff();
        break;
      case PULT_SOUND:
        relaySwitchOff(t);
        break;
      case PULT_VOLDOWN:
        //tuneLight(false, CANNEL_CW);
        break;
      case PULT_VOLUP:
        //tuneLight(true, CANNEL_CW);
        break;
      case PULT_FASTBACK:
        //tuneLight(false, CANNEL_NW);
        break;
      case PULT_FASTFORWARD:
        //tuneLight(true, CANNEL_NW);
        break;
      case PULT_PREV:
        //tuneLight(false, CANNEL_WW);
        break;
      case PULT_NEXT:
        //tuneLight(true, CANNEL_WW);
        break;
      case PULT_SLOW: //ultra low
        setOneBand(CANNEL_CW, 0);
        setOneBand(CANNEL_NW, 0);
        setOneBand(CANNEL_WW, 64);
        //logg.logging("SLOW");
        break;
      case PULT_ZOOM: //low
        setOneBand(CANNEL_CW, 64);
        setOneBand(CANNEL_NW, 64);
        setOneBand(CANNEL_WW, 64);
        break;
      case PULT_STOP: //middle
        setOneBand(CANNEL_CW, 128);
        setOneBand(CANNEL_NW, 128);
        setOneBand(CANNEL_WW, 128);
        break;
      case PULT_PAUSE: //full
        setOneBand(CANNEL_CW, 255);
        setOneBand(CANNEL_NW, 255);
        setOneBand(CANNEL_WW, 255);
        break;
      }
      break;

    case WEB_EVENT:
      switch (ev.button)
      {
      case PULT_1:
        relaySet(0, ev.count > 0);
        break;
      case PULT_2:
        relaySet(1, ev.count > 0);
        break;
      case PULT_3:
        //logg.logging("here");
        relaySet(2, ev.count > 0);
        swcLight(ev.count > 0);
        break;
      case PULT_4:
        display.showString("Comp. OFF");
        relaySwitch(3, t);
        break;
      case WEB_CANNEL_CW:
        setOneBand(CANNEL_CW, ev.count);
        break;
      case WEB_CANNEL_NW:
        setOneBand(CANNEL_NW, ev.count);
        break;
      case WEB_CANNEL_WW:
        setOneBand(CANNEL_WW, ev.count);
        break;
      }

      break;
    }
  }
}

void AppData::setOneBand(uint8_t cannel, uint8_t val)
{
   lamp.setOne(cannel, val);
  if (conf.lamp_on && mqtt) mqtt->updateState(cannel+100,val);
}

void AppData::swcLight(boolean state)
{
  if (state)
    lamp.on();
  else
    lamp.off();
  
}

void AppData::tuneLight(boolean dir, uint8_t cannel)
{
  if (dir)
    lamp.colorUp(cannel);
  else
    lamp.colorDown(cannel);
}

void AppData::logConf()
{
  conf.print();
}

boolean AppData::update_time_from_inet()
{
  WiFiUDP *ntpUDP;
  NTPClient *timeClient;
  ntpUDP = new WiFiUDP();

  timeClient = new NTPClient(*ntpUDP, ntpServer, 3600 * TIME_OFFSET, 60000 * 60 * 24);
  timeClient->begin();

  if (timeClient->forceUpdate())
  {
    DateTime d(timeClient->getEpochTime());
    rtc.adjust(d);
    fast_time_interval = false;
    logg.logging("Success update time from inet. Time is :" + rtc.now().timestamp());
  }
  else
  {
    fast_time_interval = true;
  }

  timeClient->end();
  delete timeClient;
  delete ntpUDP;
}