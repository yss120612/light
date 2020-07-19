
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
    void test(String txt);
    void loop(unsigned long t);
    void showString(String s,String s1="",String s2="");
    private:
    U8G2_SH1106_128X64_VCOMH0_1_HW_I2C u8g2=U8G2_SH1106_128X64_VCOMH0_1_HW_I2C(U8G2_R2,U8X8_PIN_NONE);
    boolean display_on;
    unsigned long display_timer;
    String toShow,toShow1,toShow2;
    //int counter;
};

#endif