#ifndef _DATA_h
#define _DATA_h
#include <Arduino.h>
#include "Relays.h"
#include "Settings.h"

class AppData
{
public:
    AppData();
    void relayOff(unsigned long t);                 //просто все выключить
    void relaySwitchOff(unsigned long t);           //все выключить + щелкнуть по кнопкам
    void relaySwitch(uint8_t i, unsigned long tm);  //все , кто не кнопки - переворачиваемся
    void relaySet(uint8_t i, boolean st);           //устанавливаем i тое реле в st
    boolean isOn(uint8_t i);                        //конкретное
    boolean isOn();                                 //хоть одно (не кнопка)
    void setup();
    void loop(unsigned long);

private:
    Relay relays[4] = {Relay(RELAY1), Relay(RELAY2), Relay(RELAY3), Relay(RELAY4)};
    const uint8_t lgh = sizeof(relays) / sizeof(Relay);
};

#endif