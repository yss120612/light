#ifndef _WEATHERTASK_h
#define _WEATHERTASK_h

#include <Task.h>
#include "Settings.h"
#include "esp_http_client.h"
//#include "Events.h"
#define weather_request  "http://api.openweathermap.org/data/2.5/weather?q=Irkutsk&appid=7d7b4c4884dc5a5c839debe4488d1712&units=metric"
#define forecast_request   "http://api.openweathermap.org/data/2.5/forecast?q=Irkutsk&appid=7d7b4c4884dc5a5c839debe4488d1712&units=metric"
#define WEATHER_INTERVAL 1000*60*10 //10 minutes

class WeatherTask: public Task{
public:    
WeatherTask(const char *name, uint32_t stack, QueueHandle_t q, EventGroupHandle_t f):Task(name, stack){que=q;flg=f;};

protected:
void cleanup() override;
void setup() override;
void loop() override;
void requestURL();

static esp_err_t static_weather_event_handler(esp_http_client_event_t *evt);
esp_err_t weather_event_handler(esp_http_client_event_t *evt);

void sent();

QueueHandle_t que;
EventGroupHandle_t flg;
//MessageBufferHandle_t _mess;
esp_http_client_config_t config;
char * buffer;
int len;
};
#endif 