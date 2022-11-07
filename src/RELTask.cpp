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
      if (relay[i].isButton()) continue;
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
        ev.state= DISP_EVENT;
        ev.button=11+idx;
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
    case 2:
    case 3:
    case 4:
    if (!relay[comm-1].isButton())
    {
      relay[comm-1].setState(data>0);
      if (!act) save(comm-1);
    }
    else{
      arm(comm-1);
    }
    break;
    case 11:
    case 12:
    case 13:
    case 14:
      if (relay[comm-11].isButton()) {
        arm(comm-11);
      }else{
      relay[comm-11].swc();
      save(comm-11);
      }
      break;
    case 20:
    Serial.println("All off");
    for (uint8_t i=0;i<4;i++){
    if (!relay[i].isButton())
    {
      relay[i].setOff();
      save(i);
    }
    }
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
