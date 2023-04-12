#ifndef _BTNTASK_h
#define _BTNTASK_h
#include <Arduino.h>
#include "Task.h"
#include "Settings.h"
#include <functional>


struct button_t {
  uint8_t pin;
  uint8_t xdbl;
  bool level;
  bool pressed;
  long presstime;
  long clicktime;
};

class BTNTask: public Task{
    
public:    
BTNTask(const char *name, uint32_t stack, QueueHandle_t q):Task(name, stack){que=q; };

protected:
bool add(uint8_t pin, bool level);
void cleanup() override;
void setup() override;
void loop() override;

//static void _isr(BTNTask *_this);
void _isr();
button_t btns[4];
QueueHandle_t que;
 
};



#endif