#include "BandLED.h"
#include "BandLED.h"
#include "Log.h"

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
    ledcWrite(CANNEL_CW, 0);
    ledcWrite(CANNEL_NW, 0);
    ledcWrite(CANNEL_WW, 0);
    //conf.cycle=0;
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
    //conf.lamp_on=true;
    //conf.save();
    //conf.changed();
    //conf.force_story();
    refresh();
}


void BandLED::off()
{
    ledcWrite(CANNEL_CW, 0);
    ledcWrite(CANNEL_NW, 0);
    ledcWrite(CANNEL_WW, 0);
    //conf.lamp_on=false;
    //conf.force_story();
    refresh();
}

void BandLED::colorUp(uint8_t cannel)
{
    if (!conf.lamp_on) return;
    switch (cannel)
    {
    case CANNEL_CW:
    if (conf.cw<64){
        conf.cw=64;
    }else if (conf.cw<128)    {
        conf.cw=128;
    }else if (conf.cw<192)    {
        conf.cw=192;
    }else if (conf.cw<255)    {
        conf.cw=255;
    }
    break;

    case CANNEL_NW:
    if (conf.nw<64){
        conf.nw=64;
    }else if (conf.nw<128)    {
        conf.nw=128;
    }else if (conf.nw<192)    {
        conf.nw=192;
    }else if (conf.nw<255)    {
        conf.nw=255;
    }
    break;

    case CANNEL_WW:
    if (conf.ww<64){
        conf.ww=64;
    }else if (conf.ww<128)    {
        conf.ww=128;
    }else if (conf.ww<192)    {
        conf.ww=192;
    }else if (conf.ww<255)    {
        conf.ww=255;
    }
    break;
    }
    conf.force_story();
    refresh();
}

void BandLED::colorDown(uint8_t cannel)
{
    if (!conf.lamp_on) return;
    switch (cannel)
    {
    case CANNEL_CW:
    if (conf.cw>192){
        conf.cw=192;
    }else if (conf.cw>128)    {
        conf.cw=128;
    }else if (conf.cw>64)    {
        conf.cw=64;
    }else if (conf.cw>0)    {
        conf.cw=0;
    }
    break;

    case CANNEL_NW:
    if (conf.nw>192){
        conf.nw=192;
    }else if (conf.nw>128)    {
        conf.nw=128;
    }else if (conf.nw>64)    {
        conf.nw=64;
    }else if (conf.nw>0)    {
        conf.nw=0;
    }
    break;

    case CANNEL_WW:
    if (conf.ww>192){
        conf.ww=192;
    }else if (conf.ww>128)    {
        conf.ww=128;
    }else if (conf.ww>64)    {
        conf.ww=64;
    }else if (conf.ww>0)    {
        conf.ww=0;
    }
    break;
    }
    //conf.save();
    conf.force_story();
    refresh();
}


void BandLED::refresh(){
    if (conf.lamp_on){
     force_refresh();
    }
    
}


void BandLED::force_refresh(){
     ledcWrite(CANNEL_CW, conf.cw);
     ledcWrite(CANNEL_NW, conf.nw);
     ledcWrite(CANNEL_WW, conf.ww);
}



void BandLED::setOne(uint8_t cannel, uint8_t value)
{
    
    //if (!conf.lamp_on) return;
   // logg.logging("CHANNEL="+String(cannel)+ "Value="+String(value));
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
    conf.force_story();
    //выкл или вкл для записи состояния
    refresh();
}
