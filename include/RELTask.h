#ifndef _RELTASK_h
#define _RELTASK_h
#include <Arduino.h>
#include "Task.h"
#include <esp_timer.h>
#include "Settings.h"
#include "Relay.h"


//#define LED_PIN GPIO_NUM_32
//const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};




class RELTask: public Task{
public:   
    RELTask(const char *name, uint32_t stack,QueueHandle_t q,bool lv=LOW):Task(name, stack){_level=lv;}
    
   
    //blinkmode_t get_blinkmode(uint8_t idx) {return led[idx]->getMode();}
protected:
    //void setLedMode(uint8_t ledN, blinkmode_t bm);
    void cleanup() override;
    void setup() override;
    void loop() override;
    void timerCallback();
    void arm(uint8_t i);
    int32_t last_time;
    uint16_t _step;
    Relay * relay[4]; 
    esp_timer_handle_t _timer;
    //bool need_timer;
    bool _level;
    
};


#endif 