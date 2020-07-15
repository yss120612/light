#ifndef _EVENTS_h
#define _EVENTS_h
#include <Arduino.h>
#include <QList.h>
#include "Config.h"
#include "Settings.h"

enum buttonstate_t : uint8_t { BTN_CLICK, BTN_LONGCLICK, PULT_BUTTON, WEB_EVENT };

struct event_t {
    buttonstate_t state;
    uint8_t button;
    uint8_t count;
    volatile long wait_time;
  };



class Events{
public:    

Events(){};    
bool have_event(){return _events.size()>0;};
bool count_event(){return _events.size();};
bool getEvent(event_t * e, long ms);
void putPultEvent(uint8_t n);
void putWebEvent(uint8_t n, uint8_t nn);
void putEvent(event_t * e);
void putEvent(uint8_t idx, event_t * e);
event_t * getEvent(uint8_t idx);

private:
QList<event_t> _events;
};



#endif