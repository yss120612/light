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
  
  // if (xTaskNotifyWait(0, 0, &command, pdMS_TO_TICKS(500)))
  // {
  //   notify_t notify;
  //   memcpy(&notify,&command,sizeof(notify));
  //   //readPacket(command,&comm,&val,&data);
  //   switch (notify.title)
  //   {
  //     case 1:
  //     case 2:
  //     case 3:
  //     case 4:
  //       showString("Switched","relay "+String(notify.title)+" is " + (notify.packet.value>0?"ON":"OFF"),"");
  //     break;
  //     case 11:
  //     case 12:
  //     case 13:
  //     case 14:
  //       //showString("Switched","relay="+comm-10," is swc!");
  //       showString("Switched","relay="+String(notify.title-10)," is swc!");
  //     break;
  //   }
  // }
  //uint8_t s=xMessageBufferReceive( mess,buf,sizeof(buf), pdMS_TO_TICKS(500));
  uint8_t s=xMessageBufferReceive( mess,buf,sizeof(buf), portMAX_DELAY);
  if (s)
  {
    String str(buf,s);
    if (str.length()<1) return;
    switch (str[0]) {
      case 'A':
        showString(str.substring(1,str.indexOf('*')),str.substring(str.indexOf('*')+1,str.lastIndexOf('*')),str.substring(str.lastIndexOf('*')+1,str.length()));
      break;
      case 'B':
        showString("Switched","relay "+str.substring(1,str.indexOf('*'))+" is ",str.substring(str.indexOf('*')+1,str.lastIndexOf('*')));
      break;
      case 'C':
        showString(str.substring(1,str.indexOf('*')),"relay "+str.substring(1,str.indexOf('*'))+" is ",str.substring(str.indexOf('*')+1,str.lastIndexOf('*')));
      break;
    }
   // showString(str.substring(0,str.indexOf('*')),str.substring(str.indexOf('*')+1,str.lastIndexOf('*')),str.substring(str.lastIndexOf('*')+1,str.length()));
    //showString(str.substring(0,str.indexOf('*')),str.substring(str.indexOf('*')+1,str.lastIndexOf('*')),str.substring(str.lastIndexOf('*')+1,str.length()));
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
      u8g2.setFont(u8g2_font_8x13_tf);
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

