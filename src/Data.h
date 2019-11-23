#ifndef _DATA_h
#define _DATA_h
#include <Arduino.h>

class AppData
{
    public:
    AppData();
    void relayOff();
    boolean relay1;
    boolean relay2;
    boolean relay3;
};

#endif