#ifndef __BUTTONS_H
#define __BUTTONS_H

//#define INTR_EXCLUSIIVE

#ifdef INTR_EXCLUSIIVE
extern "C" {
#include <user_interface.h>
}
#endif
#include <Arduino.h>
#include <QList.h>
#include "Config.h"
#include "Settings.h"
#include "Events.h"


struct __packed button_t {
  uint8_t pin : 4;
  uint8_t xdbl : 4;
  bool level : 1;
  bool paused : 1;
  bool pressed : 2;
  //bool dblclickable : 1;
  volatile uint16_t duration;
};


class Buttons {
public:


Buttons()  {
_isrtime=0;
#ifdef INTR_EXCLUSIIVE
    ETS_GPIO_INTR_ATTACH(_isr, this);
#endif
  }

  uint8_t add(uint8_t pin, bool level);
  void pause(uint8_t index);
  void resume(uint8_t index);
  void setEvents(Events * es){_evptr=es;}
protected:
  QList<button_t> _btns;
  Events * _evptr;    

  void cleanup(void *ptr);
  bool match(uint8_t index, const void *t);

  static void _isr(Buttons *_this);
  virtual void onChange(buttonstate_t state, uint8_t button, uint8_t cnt=0, long wt=0);
  
  uint32_t _isrtime;
};



#endif
