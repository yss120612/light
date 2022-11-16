#ifndef _EVENTS_h
#define _EVENTS_h
#include <Arduino.h>
#include <QList.h>
#include "Config.h"
#include "Settings.h"



struct event_t {
    buttonstate_t state;
    uint16_t button;
    uint8_t count;
    //int8_t type;
    uint32_t data;
  //  volatile long wait_time;
  };



// class Events{
// public:    

// Events(){};    
// bool have_event(){return _events.size()>0;};
// bool count_event(){return _events.size();};
// bool getEvent(event_t * e, long ms);
// void putPultEvent(uint8_t p, uint8_t n);
// void putPultEvent(uint8_t p, uint8_t n, int8_t t);
// void putWebEvent(uint8_t n, uint8_t nn);
// void putEvent(event_t * e);
// void putEvent(uint8_t idx, event_t * e);
// event_t * getEvent(uint8_t idx);

// private:
// QList<event_t> _events;
// };



#endif