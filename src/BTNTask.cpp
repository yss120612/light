#include "BTNTask.h"
#include "Events.h"

void BTNTask::setup()
{
 for (uint8_t i = 0; i < sizeof(btns); ++i) {
    btns[i].pin=0;
 }
}


void BTNTask::loop()
{
 
}

void BTNTask::cleanup()
{
for (uint8_t index=0;index<sizeof(btns);index++) {
    if (btns[index].pin>0) detachInterrupt(btns[index].pin);
  } 

}

bool BTNTask::add(uint8_t i, bool level) {
  if(i>=sizeof(btns) || bpins[i]==0) return false;
  
  btns[i].pin = bpins[i];
  btns[i].level = level;
  btns[i].paused = false;
  btns[i].pressed = false;
  btns[i].duration = 0;
  btns[i].xdbl=0;
  pinMode(btns[i].pin, level ? INPUT : INPUT_PULLUP);
  attachInterrupt(btns[i].pin,std::bind(&BTNTask::_isr, this, std::placeholders::_1), CHANGE);
  return true;
}

void ICACHE_RAM_ATTR BTNTask::_isr() {
  
    unsigned long ms=millis();
    uint32_t time = ms - _isrtime;
    //uint32_t inputs = GPI;
    for (uint8_t i = 0; i < sizeof(btns); ++i) {
      if (btns[i].pin==0) continue;

        if (time + btns[i].duration < 0xFFFF)
            btns[i].duration += time;
        else
          btns[i].duration = 0xFFFF;

      if ( (digitalRead(btns[i].pin) & 0x01) == _this->_btns[i].level) { // Button pressed
        if (! btns[i].pressed) {
          if (btns[i].duration > DBLCLICK_TIME) 
          {
           btns[i].xdbl=0;
          }
          btns[i].duration = 0;
          btns[i].pressed = true;
        }
      } else { // Button released
        if (btns[i].pressed) {
        
          if (btns[i].duration >= LONGCLICK_TIME) {
            onChange(BTN_LONGCLICK, i,_this->_btns[i].xdbl);
            btns[i].xdbl = 0;
          } else if (_this->_btns[i].duration >= CLICK_TIME) {
             btns[i].xdbl += 1;
             onChange(BTN_CLICK, i,_this->_btns[i].xdbl, ms);
          } else {
            
          }
          btns[i].pressed = false;
          btns[i].duration = 0;
        }
      }
    }
  _isrtime = millis();
}

void BTNTask::onChange(buttonstate_t state, uint8_t button, uint8_t cnt, long m){
event_t evt;
evt.button=button;
evt.state=state;
evt.count=cnt;
evt.wait_time=m;
int idx=-1;
if (cnt>1){
  for (int k=0;k<_evptr->count_event();k++){
    if (_evptr->getEvent(k)->button==button && _evptr->getEvent(k)->state==BTN_CLICK && _evptr->getEvent(k)->count==(cnt-1)) idx=k;
  }
  if (idx>=0) {
  _evptr->putEvent(idx,&evt);
  return;
  }
}
_evptr->putEvent(&evt);
}