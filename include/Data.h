#ifndef _DATA_h
#define _DATA_h

#include <Arduino.h>


//#include "BandLED.h"
#include "Settings.h"
//include "Config.h"
#include "buttons.h"
#include "Events.h"
#include "Config.h"

#include "MQTTclient.h"

//class MqttClient;

class AppData
{
public:
    AppData();
    void relayOff();                 //просто все выключить
    void relaySwitchOff(unsigned long t);           //все выключить + щелкнуть по кнопкам
    void relaySwitch(uint8_t i, unsigned long tm);  //все , кто не кнопки - переворачиваемся
    void relaySet(uint8_t i, boolean st);           //устанавливаем i тое реле в st
    boolean isOn(uint8_t i);                        //конкретное
    boolean isOn();                                 //хоть одно (не кнопка)
    void setup(MqttClient * mq);
    void loop(unsigned long);
    uint8_t getCW();
    uint8_t getNW();
    uint8_t getWW();
    void setOneBand(uint8_t cannel, uint8_t val);
    void swcLight(boolean state);
    void tuneLight(boolean dir,uint8_t cannel);
    void logConf();
    void ProcessEvents(unsigned long t);
    void ir_sleep();
    void putWebEvent(uint8_t n, uint8_t nn){evts.putWebEvent(n,nn);};
    void getI2Cdevices();
private:
    
   

   // boolean update_time_from_inet();
    Events evts;
    Buttons btns;
    //IRreceiver  ir=IRreceiver(IRPIN);
    //Relay relays[4] = {Relay(RELAY1), Relay(RELAY2), Relay(RELAY3), Relay(RELAY4)};
    //const uint8_t lgh = sizeof(relays) / sizeof(Relay);
    //BandLED lamp;
    //RTC_DS3231 rtc;
    MqttClient * mqtt;
    boolean fast_time_interval;
    unsigned long last_tsync;
    unsigned long learn_commang;//10 минут отображаем комманды с пульта
   // Display display;
    
};

#endif