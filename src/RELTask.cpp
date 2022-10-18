#include "RELTask.h"

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


 

  // ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};
  for (uint8_t i = 0; i < 4; i++)
    if (rpins[i] > 0)
    {
      relay[i] = new Relay(rpins[i], _level);
      relay[i] ->setup(NULL,false);
    }
    else
      relay[i] = NULL;
  ///last_time = 0;

  //setLedMode(0, BLINK_05HZ);
  //setLedMode(1, BLINK_OFF);
}

void RELTask::arm(uint8_t i) {
 if (i>=4 || !relay[i])  return;
 relay[i]->arm();
if (esp_timer_start_periodic(_timer, 500000) != ESP_OK) {
           return;
         }
}

void RELTask::loop()
{
  uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    uint8_t comm=command>>16 & 0x0000FFFF;
    uint8_t act=command & 0x0000FFFF;
    switch (comm)
    {
    case 1:
      relay[0]->set(act>0);
        break;
    case 2:
      relay[1]->set(act>0);
      break;
    case 3:
      relay[2]->set(act>0);
      break;
    case 4:
      arm(3);
      break;
    case 11:
      relay[0]->swc();
      break;
    case 12:
      relay[1]->swc();
      break;
    case 13:
      relay[2]->swc();
      break;
    case 14:
      arm(3);
      break;
    case 20:
      relay[0]->setOff();
      relay[1]->setOff();
      relay[2]->setOff();
      relay[3]->setOff();
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
      delete (relay[i]);
    }
  }
}

void RELTask::timerCallback()
{
for (uint8_t i = 0; i < 4; i++)
    if (rpins[i]>0)
    {
      relay[i] -> disarm();
    }
esp_timer_stop(_timer);    
 
}
