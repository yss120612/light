#include "BandLCD.h"
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
    off();
}

void BandLED::on()
{
    cw=128;
    ww=128;
    nw=128;
    refresh();
}


void BandLED::off()
{
    cw=0;
    ww=0;
    nw=0;
    refresh();
}

uint8_t BandLED::getCW(){
    return cw;
}

uint8_t BandLED::getNW(){
    return nw;
}

uint8_t BandLED::getWW(){
    return ww;
}

void BandLED::refresh(){
    ledcWrite(PIN_CW, cw);
    ledcWrite(PIN_NW, nw);
    ledcWrite(PIN_WW, ww);
}

void BandLED::setOne(uint8_t cannel, uint8_t value)
{
    switch (cannel){
        case CANNEL_CW:
            cw=value;
        break;
        case CANNEL_NW:
            nw=value;
        break;
        case CANNEL_WW:
            ww=value;
        break;
    }
    refresh();
}
