#ifndef _CONFIG_h
#define _CONFIG_h
#include <Arduino.h>

class Config{
public:    
Config();    
void save();
void load();
uint8_t cw;
uint8_t nw;
uint8_t ww;
uint16_t cycle;

boolean lamp_on;
private:

void loadDefaults();
};


extern Config conf;

#endif