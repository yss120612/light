#include "Events.h"


bool Events::getEvent(event_t * e, long ms){
  
  if (have_event())
  {
    *e   = _events.front();
    _events.pop_front();
    if (e->state==BTN_CLICK && ms-e->wait_time<DBLCLICK_TIME){
    _events.push_back(*e);    
    return false;
    }
    return   true;
  }
  return   false;
}

void Events::putEvent(event_t *e){
  
  _events.push_back(*e);
}

void Events::putPultEvent(uint8_t p, uint8_t n){
event_t e;
e.state=PULT_BUTTON;
e.button=n;
e.count=p;
e.type=-1;
_events.push_back(e);
}

void Events::putPultEvent(uint8_t p, uint8_t n, int8_t t){
event_t e;
e.state=PULT_BUTTON;
e.button=n;
e.count=p;
e.type=t;
_events.push_back(e);
}


void Events::putWebEvent(uint8_t n, uint8_t nn){
    event_t e;
    e.state=WEB_EVENT;
    e.button=n;
    e.count=nn;
    _events.push_back(e);
}

void Events::putEvent(uint8_t idx, event_t *e){
  if (idx>=0 && idx < _events.size()>idx) _events[idx]=*e;
}



event_t * Events::getEvent(uint8_t idx){
    if (idx>=0 && idx < _events.size()>idx) return &_events[idx];
    return NULL;
};