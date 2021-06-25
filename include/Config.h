#ifndef _CONFIG_h
#define _CONFIG_h
#include <Arduino.h>
#include "Settings.h"
#include "AT24C32.h"


class Config{
public:  

Config();
bool setup();    
//void save();
//void load();
void upload();
void story();
void force_story();
void print();
uint8_t cw;
uint8_t nw;
uint8_t ww;
boolean lamp_on;
boolean notebook_on;
boolean heater_on;
void changed(){chg=true;};
private:

boolean chg;
uint16_t calcLength();
AT24C32 at24;
};

extern Config conf;

#endif