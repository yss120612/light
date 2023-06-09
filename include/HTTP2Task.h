#ifndef _HTTP2TASK_h
#define _HTTP2TASK_h
#include <HTTPTask.h>

#include "Settings.h"

class HTTP2Task: public HTTPTask{
public:
HTTP2Task(const char *name, uint32_t stack,QueueHandle_t q,EventGroupHandle_t f,MessageBufferHandle_t mess):HTTPTask(name, stack,q,f,mess){};
protected:
void setup() override;
void loop() override;

void handleW2A(AsyncWebServerRequest * request);
void handleA2W(AsyncWebServerRequest * request);
void var(String n, String v);
String getI2Cdevices();

};
#endif
