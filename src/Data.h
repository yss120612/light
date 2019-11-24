#ifndef _DATA_h
#define _DATA_h
#include <Arduino.h>
#include "Relays.h"
#include "Settings.h"

class AppData
{
    public:
    AppData();
    void relayOff();
    void relaySwitch(uint8_t i);
    boolean isOn(uint8_t i);
    private:
    Relay relays [4]={Relay(RELAY1),Relay(RELAY2),Relay(RELAY3),Relay(RELAY4)};
};

#endif