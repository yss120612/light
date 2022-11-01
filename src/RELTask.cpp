#include "RELTask.h"
#include "Events.h"

void RELTask::setup()
{

  esp_timer_create_args_t timer_cfg = {
      .callback = (esp_timer_cb_t)&RELTask::timerCallback,
      .arg = this};

  if (esp_timer_create(&timer_cfg, &_timer) != ESP_OK)
  {
    _timer = NULL;
    //ESP_LOGE(TAG, "Error creating blinker timer!\r\n");
    return;
  }

    event_t ev;
    ev.state= MEM_EVENT;
    
  
  for (uint8_t i = 0; i < 4; i++)
    if (rpins[i] > 0)
    {
      //relay[i] = Relay();
      relay[i].setup(rpins[i],i<3?RELTYPE_SWICH:RELTYPE_BUTTON, _level);
      ev.button=104+i;
      xQueueSend(que,&ev,portMAX_DELAY);
      delay(100);
    }
    else
    {
      //relay[i] = NULL;
    }
  
}

void RELTask::arm(uint8_t i) {
 if (i>=4)  return;
 relay[i].arm();
if (esp_timer_start_periodic(_timer, 500000) != ESP_OK) {
           return;
         }
}

void RELTask::save(uint8_t idx){
        event_t ev;
        ev.state=MEM_EVENT;
        ev.button=204+idx;
        ev.count=relay[idx].isOn();
        xQueueSend(que,&ev,portMAX_DELAY);        
}

void RELTask::loop()
{
  uint32_t command;
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    uint8_t comm,act;
    uint16_t data;
    readPacket(command,&comm,&act,&data);
    switch (comm)
    {
    case 1:
      relay[0].setState(data>0);
      if (!act) save(0);
        break;
    case 2:
      relay[1].setState(data>0);
      if (!act) save(1);
      break;
    case 3:
      relay[2].setState(data>0);
      if (!act) save(2);
      break;
    case 4:
      arm(3);
      //if (!act) save(0);
      break;
    case 11:
      relay[0].swc();
      save(0);
      break;
    case 12:
      relay[1].swc();
      save(1);
      break;
    case 13:
      relay[2].swc();
      save(2);
      break;
    case 14:
      //arm(3);
      //save(0);
      
       event_t ev;
    ev.state= MEM_EVENT;
    for (uint8_t i = 0; i < 4; i++)
    if (rpins[i] > 0)
    {
      ev.button=104+i;
      xQueueSend(que,&ev,portMAX_DELAY);
      delay(100);
    }
      break;
    case 20:
    
      relay[0].setOff();
      save(0);
      relay[1].setOff();
      save(1);
      relay[2].setOff();
      save(2);
      relay[3].setOff();
      //save(3);
    break;
  }
  }
}

void RELTask::cleanup()
{
  if (_timer)
  {
    esp_timer_stop(_timer);
    esp_timer_delete(_timer);
    for (uint8_t i = 0; i < 4; i++)
    {
      if (rpins[i] <= 0)
        break;
      gpio_reset_pin((gpio_num_t)rpins[i]);
      //  delete (relay[i]);
    }
  }
}

void RELTask::timerCallback()
{
for (uint8_t i = 0; i < 4; i++)
    if (rpins[i]>0)
    {
      relay[i].disarm();
    }
esp_timer_stop(_timer);    
 
}
