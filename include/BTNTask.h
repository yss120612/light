#ifndef _BTNTASK_h
#define _BTNTASK_h
#include <Arduino.h>
#include "Task.h"
#include "Settings.h"
#include <functional>


struct __packed button_t {
  uint8_t pin : 4;
  uint8_t xdbl : 4;
  bool level : 1;
  bool paused : 1;
  bool pressed : 2;
  //bool dblclickable : 1;
  volatile uint16_t duration;
};

class BTNTask: public Task{
    
public:    
BTNTask(const char *name, uint32_t stack, QueueHandle_t q, bool lv=HIGH ):Task(name, stack){que=q;};

protected:
bool add(uint8_t pin, bool level);
void cleanup() override;
void setup() override;
void loop() override;
void IRAM_ATTR _isr();
void onChange(buttonstate_t state, uint8_t button, uint8_t cnt, long m=0);
button_t btns[sizeof(bpins)];
QueueHandle_t que;
unsigned long _isrtime;

};



#endif