#include "DISPTask.h"

void DISPTask::setup()
{
 u8g2.begin();
 display_on=false;
 display_timer=0;
}


void DISPTask::loop()
{
  uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, 100))
  {
    uint8_t comm,val;
    uint16_t data;
    readPacket(command,&comm,&val,&data);
    switch (comm)
    {
      case 1:
      case 2:
      case 3:
      case 4:
        showString("Switched","relay "+String(comm)+" is " + (data>0?"ON":"OFF"),"");
      break;
      case 11:
      case 12:
      case 13:
      case 14:
        //showString("Switched","relay="+comm-10," is swc!");
        showString("Switched","relay="+String(comm-10)," is swc!");
      break;
    }
  }

  if (!display_on && display_timer==0) return;  
  if (display_on) 
{
  drawText();
  display_on=false;
  display_timer=millis();
}
else  if (millis()-display_timer>DISPLAY_ON_TIME){
   u8g2.clear();
   u8g2.sleepOn();
   display_timer=0;
}
}

void DISPTask::drawText(){
  
  u8g2.sleepOff();
  u8g2.setFont(u8g2_font_8x13_t_cyrillic);
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 20);
    u8g2.print(toShow);
    if (toShow1!="")
    {
      //u8g2.setFont(u8g2_font_timB12_tn);
    u8g2.setCursor(0, 40);
    u8g2.print(toShow1);
    }
    if (toShow2!="")
    {
      //u8g2.setFont(u8g2_font_timB12_tr);
    u8g2.setCursor(0, 60);
    u8g2.print(toShow2);
    }
  } while ( u8g2.nextPage() );
}

void DISPTask::showString(String s,String s1,String s2){
  toShow=s;
  toShow1=s1;
  toShow2=s2;
  display_on=true;
}


void DISPTask::cleanup(){
u8g2.setPowerSave(true);
u8g2.sleepOn();

}

