#include "Config.h"
#include <SPIFFS.h>
#include "Log.h"

Config::Config(){
loadDefaults();
}

void Config::load(){
File file = SPIFFS.open("/config.txt");
  if(!file){
    logg.logging("Failed to open config file for reading");
    return;
  }
  while(file.available()){
       // Serial.write(file.read();
    }
}

void Config::loadDefaults(){
    cw=128;
    nw=128;
    ww=128;
    lamp_on=false;
    save();
}

void Config::save(){

}

Config conf;