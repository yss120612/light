#ifndef _RTCTASK_h
#define _RTCASK_h
#include "Task.h"
#include "Settings.h"
#include <WiFiUdp.h>
#include <NtpClient.h>
#include "RTClib.h"

//#define LED_PIN GPIO_NUM_32
//const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};




class RTCTask: public Task{
public:   
    RTCTask(const char *name, uint32_t stack,QueueHandle_t q,EventGroupHandle_t f):Task(name, stack){que=q;flg=f;}
    
   
    //blinkmode_t get_blinkmode(uint8_t idx) {return led[idx]->getMode();}
protected:
    //void setLedMode(uint8_t ledN, blinkmode_t bm);
    bool update_time_from_inet();
    void cleanup() override;
    void setup() override;
    void loop() override;
    int32_t last_sync;
    boolean fast_time_interval; 
    QueueHandle_t que;
    EventGroupHandle_t flg;
    RTC_DS3231 * rtc;
    const char * NTPServer = "pool.ntp.org";
};


#endif 