#ifndef _LEDTASK_h
#define _LEDTASK_h
#include <Arduino.h>
#include "Task.h"
#include <esp_timer.h>
#include <driver/ledc.h>
#include "Led.h"
#include "Settings.h"
//#define LED_PIN GPIO_NUM_32

const uint8_t pins[]={LED,0,0,0};



class LEDTask: public Task{
public:   
    LEDTask(const char *name, uint32_t stack,QueueHandle_t q,bool lv=LOW):Task(name, stack){que=q;_level=lv;}
    
   
    blinkmode_t get_blinkmode(uint8_t idx) {return led[idx]->getMode();}
protected:
    void setLedMode(uint8_t ledN, blinkmode_t bm);
    void cleanup() override;
    void setup() override;
    void loop() override;
    void timerCallback();
    int32_t last_time;
    uint16_t _step;
    Led * led[4]; 
    esp_timer_handle_t _timer;
    //bool need_timer;
    bool _level;
    QueueHandle_t que;
};


#endif 