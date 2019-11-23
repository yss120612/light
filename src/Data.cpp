#include "Data.h"

AppData::AppData(){
    relayOff();
}

void AppData::relayOff(){
    relay1=false;
    relay2=false;
    relay3=false;
}