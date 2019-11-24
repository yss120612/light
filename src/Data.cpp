#include "Data.h"


AppData::AppData(){
   
}

void AppData::relayOff(){
    for (uint8_t i=0;i<3;i++) relays[i].setOff();
}

void AppData::relaySwitch(uint8_t i){
    if (i<sizeof(relays)/sizeof(relays[0]))    relays[i].swc();
}

boolean AppData::isOn(uint8_t i){
        return relays[i].isOn();
}