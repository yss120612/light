#include "display.h"

Display::Display(){

}

void Display::setup(){
  u8g2.begin();
  display_on=true;
}

void Display::loop(unsigned long t){
if (!display_on) return;
if (t-display_timer>DISPLAY_ON_TIME){
  display_on=false;
  u8g2.clear();
}
}

