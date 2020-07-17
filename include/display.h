
#ifndef _DISPLAY_h
#define _DISPLAY_h
#include <Arduino.h>
#include <Wire.h>
#define U8X8_HAVE_HW_I2C
#include <U8g2lib.h>

#include "Settings.h"


class Display{

    public:
    Display();
    void setup();
    void drawText();
    void loop(unsigned long t);

    private:
    U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2=U8G2_SH1106_128X64_NONAME_1_HW_I2C(U8G2_R0,U8X8_PIN_NONE);
    boolean display_on;
    unsigned long display_timer;
};

#endif