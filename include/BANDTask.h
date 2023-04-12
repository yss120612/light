#ifndef _BANDTASK_h
#define _BANDTASK_h
#include "Task.h"
#include <esp_timer.h>
#include <driver/ledc.h>
#include "Settings.h"


class BANDTask: public Task{
    
public:    
BANDTask(const char *name, uint32_t stack, QueueHandle_t q,  MessageBufferHandle_t mess, bool lv=HIGH ):Task(name, stack){que=q;_level=lv;disp_mess=mess;};

protected:
void cleanup() override;
void setup() override;
void loop() override;
//void off();
//void on();
//void swc();

//void colorUp(uint8_t cannel);
//void colorDown(uint8_t cannel);

void setOne(uint8_t cannel, uint8_t value,bool sav=true);
//void force_refresh();

//void refresh();
bool _level;
//esp_timer_handle_t _timer;
QueueHandle_t que;
MessageBufferHandle_t disp_mess;
};


#endif 