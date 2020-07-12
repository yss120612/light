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

enum buttonstate_t : uint8_t { BTN_RELEASED, BTN_PRESSED, BTN_CLICK, BTN_LONGCLICK, BTN_DBLCLICK };

const uint16_t CLICK_TIME = 20; // 20 ms.
const uint16_t LONGCLICK_TIME = 1000; // 1 sec.
const uint16_t DBLCLICK_TIME = 500; // 0.5 sec.

struct __packed button_t {
  uint8_t pin : 4;
  uint8_t xdbl : 4;
  bool level : 1;
  bool paused : 1;
  bool pressed : 2;
  //bool dblclickable : 1;
  volatile uint16_t duration;
};

struct event_t {
    buttonstate_t state;
    uint8_t button;
    uint8_t count;
    volatile long wait_time;
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
  bool have_event(){return _events.size()>0;};
  bool getEvent(event_t * e, long ms);
  void putEvent(event_t * e);
protected:
  QList<button_t> _btns;
  QList<event_t> _events;

  

  void cleanup(void *ptr);
  bool match(uint8_t index, const void *t);

  static void _isr(Buttons *_this);
  virtual void onChange(buttonstate_t state, uint8_t button, uint8_t cnt=0, long wt=0);
  

  uint32_t _isrtime;
};



#endif
