#ifndef _VOICETASK_h
#define _VOIICETASK_h

#include <Task.h>
#include "Settings.h"

class VoiceTask: public Task{
public:    
VoiceTask(const char *name, uint32_t stack, QueueHandle_t q,MessageBufferHandle_t voice_message):Task(name, stack)
{
    que=q;
    message=voice_message;
};

protected:
int receiveCMD();

void cleanup() override;
void setup() override;
void loop() override;
QueueHandle_t que;
uint8_t cmd_cnt;
uint8_t buffer[CMD_BUF_LEN]; 
void processBuffer(uint8_t * buff);
String printBuff(char *buff, uint16_t length);
//QueueHandle_t uart_queue;
MessageBufferHandle_t message;
void onMyTick();
static void onTick(TimerHandle_t tm);
void loadGroup();
void makeCommand (uint8_t command);
TimerHandle_t _timer;
bool vasya;
uint8_t first_command;
};


#endif 