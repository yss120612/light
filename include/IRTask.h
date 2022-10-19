#ifndef _IRTASK_h
#define _IRTASK_h
#include "Task.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include "Settings.h"

class IRTask: public Task{
public:    
IRTask(const char *name, uint32_t stack, QueueHandle_t q):Task(name, stack){que=q;};
protected:
void cleanup() override;
void setup() override;
void loop() override;
QueueHandle_t que;
IRrecv * irrecv;
decode_results dres;
};


#endif 