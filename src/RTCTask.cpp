#include "RTCTask.h"

void RTCTask::setup()
{
rtc= new RTC_DS3231();
rtc->begin();
fast_time_interval=true;
last_sync=0;
}

void RTCTask::cleanup()
{
delete rtc;
}

void RTCTask::loop()
{
    uint32_t command;
    if (xTaskNotifyWait(0, 0, &command, 1000))
    {
        uint8_t comm, act;
        uint16_t data;
        readPacket(command, &comm, &act, &data);
        switch (comm)
        {
        case 1:
            break;
        }
    }

    
if (xEventGroupWaitBits(flg, FLAG_WIFI, pdFALSE, pdTRUE, portMAX_DELAY) & FLAG_WIFI) {    
unsigned long t= millis();    
if (t < last_sync) last_sync=t;
  if (last_sync==0 || t - last_sync > (fast_time_interval ? SHORT_TIME : LONG_TIME))
  {
    last_sync = t;
    fast_time_interval = !update_time_from_inet();
  }
}

}

bool RTCTask::update_time_from_inet()
{
  WiFiUDP *ntpUDP;
  NTPClient *timeClient;
  ntpUDP = new WiFiUDP();

  timeClient = new NTPClient(*ntpUDP, NTPServer, 3600 * TIME_OFFSET, 60000 * 60 * 24);
  timeClient->begin();
  bool result=timeClient->forceUpdate();
  if (result)
  {
    DateTime d(timeClient->getEpochTime());
    rtc->adjust(d);
    Serial.println("Success update time from inet. Time is :" + rtc->now().timestamp());
  }
    

  timeClient->end();
  delete timeClient;
  delete ntpUDP;
  return result;
}