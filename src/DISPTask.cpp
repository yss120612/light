#include "DISPTask.h"

void DISPTask::setup()
{

 
}


void DISPTask::loop()
{
  uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    uint8_t comm=command>>16 & 0x0000FFFF;
    uint8_t act=command & 0x0000FFFF;
    switch (comm)
    {
    
  }
  }
}

void DISPTask::cleanup()
{
  
}

