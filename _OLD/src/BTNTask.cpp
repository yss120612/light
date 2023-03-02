#include <functional>
#include <FunctionalInterrupt.h>
#include "BTNTask.h"
//#include "Events.h"
#include "Settings.h"

void BTNTask::setup()
{
 for (uint8_t i = 0; i < sizeof(btns); ++i) {
    if (bpins[i]>0) add(i,blevels[i]);
    else btns[i].pin=0;
 }


}


void BTNTask::loop()
{
   unsigned long ms=millis();
   for (uint8_t i = 0; i < 4; ++i) {
      if (btns[i].pin==0) continue;
     if (btns[i].clicktime>0 && !btns[i].pressed && ms-btns[i].clicktime>DBLCLICK_TIME){
             event_t ev;
             ev.state=BTN_CLICK;  
             ev.count=btns[i].xdbl;
             ev.button=i;
             btns[i].xdbl=0;
             btns[i].clicktime=0;
             xQueueSend(que,&ev,portMAX_DELAY);
    }
   }
   delay(100);
}

void BTNTask::cleanup()
{
// for (uint8_t index=0;index<4;index++) {
//     if (btns[index].pin>0) detachInterrupt(btns[index].pin);
//   } 

}

bool BTNTask::add(uint8_t i, bool level) {
  if(i>=4 || bpins[i]==0) return false;
  
  btns[i].pin = bpins[i];
  btns[i].level = level;
  btns[i].pressed = false;
  btns[i].clicktime = 0;
  btns[i].presstime = 0;
  btns[i].xdbl=0;
  pinMode(btns[i].pin, level ? INPUT : INPUT_PULLUP);
  attachInterrupt(btns[i].pin,std::bind(&BTNTask::_isr, this), CHANGE);
  return true;
}


//void ICACHE_RAM_ATTR BTNTask::_isr(BTNTask * _this) {
void ICACHE_RAM_ATTR BTNTask::_isr() {
  
    unsigned long ms=millis();
    bool state;
   
    for (uint8_t i = 0; i < 4; ++i) {
      if (btns[i].pin==0) continue;
      state=digitalRead(btns[i].pin) & 0x01 == btns[i].level;//true=pressed false=released
      if (state==btns[i].pressed) continue;//не моя кнопень
      btns[i].pressed=state;
      if (state) {btns[i].presstime=ms;}//далее только отпускание
      if (ms-btns[i].presstime< CLICK_TIME) continue;//дребезг
      if (ms-btns[i].presstime >= LONGCLICK_TIME){//long click
        event_t ev;
        ev.state=BTN_LONGCLICK;
        ev.count=btns[i].xdbl;
        ev.button=i;
        btns[i].xdbl=0;
        btns[i].clicktime=0;
        xQueueSendFromISR(que,&ev,NULL);
      }else{//click
          btns[i].xdbl++;
          btns[i].clicktime=ms;
      }
      
    }     
}

