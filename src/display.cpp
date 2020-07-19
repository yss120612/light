#include "display.h"
#include "Log.h"
Display::Display(){

}

void Display::setup(){
  u8g2.begin();
  display_on=false;
  display_timer=0;
  //counter=0;
}

void Display::loop(unsigned long t){
if (!display_on && display_timer==0) return;  
if (display_on) 
{
  //logg.logging("T="+String(t));
  display_on=false;
  //counter++;

  drawText();
  display_timer=t;
}
else  if (t-display_timer>DISPLAY_ON_TIME){
   u8g2.clear();
   display_timer=0;
   u8g2.sleepOn();
   //logg.logging("T="+String(t)+" DT="+String(display_timer));
}
}

void Display::drawText(){
  
  u8g2.sleepOff();
  u8g2.setFont(u8g2_font_helvB12_tf);
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 20);
    u8g2.print(toShow);
    if (toShow1!="")
    {
      u8g2.setFont(u8g2_font_helvB12_tn);
    u8g2.setCursor(0, 40);
    u8g2.print(toShow1);
    }
    if (toShow2!="")
    {
      u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setCursor(0, 60);
    u8g2.print(toShow2);
    }
  } while ( u8g2.nextPage() );


  //u8g2.drawCircle(30,30,10);
  //u8g2.drawStr(30,40,"Huiii");
}

void Display::showString(String s,String s1,String s2){
  toShow=s;
  toShow1=s1;
  toShow2=s2;
  display_on=true;
}



void Display::test(String txt){
  //logg.logging("N tex="+String);
  //delay(1000);
}