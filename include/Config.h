#ifndef _CONFIG_h
#define _CONFIG_h
#include <Arduino.h>




class Config{
public:    
Config();    
void save();
void load();
void print();
uint8_t cw;
uint8_t nw;
uint8_t ww;
boolean lamp_on;
private:
};

extern Config conf;

#endif