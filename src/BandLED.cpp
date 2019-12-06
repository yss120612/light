#include "BandLED.h"
#include "BandLED.h"

BandLED::BandLED()
{
}

void BandLED::setup()
{
    ledcSetup(CANNEL_CW, FREQ, RESOLUTION);
    ledcSetup(CANNEL_NW, FREQ, RESOLUTION);
    ledcSetup(CANNEL_WW, FREQ, RESOLUTION);
    ledcAttachPin(PIN_CW, CANNEL_CW);
    ledcAttachPin(PIN_NW, CANNEL_NW);
    ledcAttachPin(PIN_WW, CANNEL_WW);
    ledcWrite(PIN_CW, 0);
    ledcWrite(PIN_NW, 0);
    ledcWrite(PIN_WW, 0);
    refresh();
}

void BandLED::swc()
{
if (conf.lamp_on){
    off();
}
else{
    on();
}
}


void BandLED::on()
{
    conf.lamp_on=true;
    conf.save();
    refresh();
}


void BandLED::off()
{
    ledcWrite(PIN_CW, 0);
    ledcWrite(PIN_NW, 0);
    ledcWrite(PIN_WW, 0);
    conf.lamp_on=false;
    conf.save();
    refresh();
}

void BandLED::refresh(){
    if (conf.lamp_on){
     ledcWrite(PIN_CW, conf.cw);
     ledcWrite(PIN_NW, conf.nw);
     ledcWrite(PIN_WW, conf.ww);
    }
    
}



void BandLED::setOne(uint8_t cannel, uint8_t value)
{
    switch (cannel){
        case CANNEL_CW:
            conf.cw=value;
        break;
        case CANNEL_NW:
            conf.nw=value;
        break;
        case CANNEL_WW:
            conf.ww=value;
        break;
    }
    //conf.save();
    //выкл или вкл для записи состояния
    refresh();
}
