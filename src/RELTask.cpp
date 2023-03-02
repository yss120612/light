#include "RELTask.h"
//#include "Events.h"

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

    // event_t ev;
    // ev.state= MEM_EVENT;
    // ev.button=1;
  
  for (uint8_t i = 0; i < 4; i++)
    if (rpins[i] > 0)
    {
      //relay[i] = Relay();
      relay[i].setup(rpins[i],RELTYPE_SWICH, _level);
      //if (relay[i].isButton()) continue;
      //ev.count=i;
      //xQueueSend(que,&ev,portMAX_DELAY);//запрос состояния реле
      //xQueueSend(que,&ev,portTICK_PERIOD_MS);
      //vTaskDelay(pdMS_TO_TICKS(500));
    }
    else
    {
      //relay[i] = NULL;
    }
  //relay[0].setOff();
  //relay[1].setOn();
  //relay[2].setOff();
  //relay[3].setOff();

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
        ev.button=10+idx;
        ev.count=relay[idx].isOn();
        xQueueSend(que,&ev,portMAX_DELAY);    
        //xQueueSend(que,&ev,portTICK_PERIOD_MS);
       // vTaskDelay(pdMS_TO_TICKS(500));  
}

void RELTask::loop()
{
  uint32_t command;
  notify_t nt;
  event_t ev;
  uint8_t i;
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    //uint8_t comm,act;
    //uint16_t data;
    memcpy(&nt,&command,sizeof(nt));
    //readPacket(command,&comm,&act,&data);
    switch (nt.title)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    if (relay[nt.title-1].isButton())
    {
      arm(nt.title-1);
    }
    else{
      relay[nt.title-1].setState(nt.packet.value>0);
      if (!nt.packet.var) save(nt.title-1);
    }
    break;
    case 11:
    case 12:
    case 13:
    case 14:
      if (relay[nt.title-11].isButton()) {
        arm(nt.title-11);
      }else{
        relay[nt.title-11].swc();
        save(nt.title-11);
      }
      break;
    case 20:
    #ifdef DEBUGG
    Serial.println("All off");
    #endif
    for (i=0;i<4;i++){
    if (!relay[i].isButton())
    {
      relay[i].setOff();
      save(i);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    }
  break;
  case 21://ask relay states
    ev.state=WEB_EVENT;
    ev.button=21;
    ev.data=relay[3].isOn()<<3 & 8 || relay[2].isOn()<<2 & 4 || relay[1].isOn()<<1 & 2 || relay[0].isOn() & 1;
    xQueueSend(que,&ev,portMAX_DELAY);
  break;
  case 23:
   
    for (i=0;i<4;i++){
    if (!relay[i].isButton())
    {
      relay[i].setState(nt.packet.value>>i & 1);
      Serial.printf("Relay%d %d \n",i,relay[i].isOn());
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
