#ifndef _RTCTASK_h
#define _RTCASK_h
#include <Arduino.h>
#include "Task.h"
//#include "Settings.h"
#include <WiFiUdp.h>
#include <NtpClient.h>
#include "RTClib.h"
#include "Settings.h"


//#define LED_PIN GPIO_NUM_32
//const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};




class RTCTask: public Task{
public:   
    RTCTask(const char *name, uint32_t stack,EventGroupHandle_t f,QueueHandle_t q,MessageBufferHandle_t m,MessageBufferHandle_t a):Task(name, stack){que=q;flg=f;disp_mess=m;alarm_mess=a;}
    
   
    //blinkmode_t get_blinkmode(uint8_t idx) {return led[idx]->getMode();}
protected:
    //void setLedMode(uint8_t ledN, blinkmode_t bm);
    //void alarm(uint8_t hh,uint8_t mm);
    //void alarm(uint8_t hh,uint8_t mm,uint8_t dw);
    void initAlarms();
    void saveAlarm(uint8_t idx);
    uint8_t findAndSetNext(DateTime dt, Ds3231Alarm2Mode mode);
    bool setupAlarm(uint8_t idx, uint8_t act, uint8_t h, uint8_t m,  period_t p);
    uint8_t refreshAlarms();
    void resetAlarms();
    
    void alarm(alarm_t &a);
 

    void alarmFired(uint8_t aNo);
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
    MessageBufferHandle_t alarm_mess;
    MessageBufferHandle_t disp_mess;
    alarm_t alarms[ALARMS_COUNT];
    //alarm_t copy_alarms[ALARMS_COUNT];
    bool init_complete;
};
#endif 