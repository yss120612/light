#ifndef __WIFITASK__
#define __WIFITASK__
#include "Task.h"
#include "Led.h"
#include <WiFi.h>
#include "Settings.h"



//const char WIFI_SSID[] = "Lite5G";
//const char WIFI_PSWD[] = "qweasdzxc";

const char WIFI_SSID[] = "Yss_GIGA";
const char WIFI_PSWD[] = "bqt3bqt3";



class WiFiTask: public Task
{
public:
WiFiTask(const char *name, uint32_t stack,QueueHandle_t q,EventGroupHandle_t f):Task(name, stack){que=q;flg=f;}
void save_blinkmode(blinkmode_t saved){saved_mode=saved;}
blinkmode_t restore_blinkmode(){return saved_mode;}
protected:

void cleanup() override;
void setup() override;
void loop() override;
void wifiOnEvent(WiFiEvent_t event);
QueueHandle_t que;
blinkmode_t saved_mode;
EventGroupHandle_t flg;

};
#endif