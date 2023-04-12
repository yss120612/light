#include "BANDTask.h"


void BANDTask::setup()
{

  // esp_timer_create_args_t timer_cfg;
  // timer_cfg.callback=NULL;
  // timer_cfg.arg=NULL;
  // if (esp_timer_create(&timer_cfg, &_timer) != ESP_OK)
  // {
  //   _timer = NULL;
  //   Serial.println("Error creating timer");
  //    return;
  // }
  // else{
  //   Serial.println("Success creating timer");
  // }
gpio_set_direction((gpio_num_t)PIN_WW, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)PIN_WW, ! _level);
gpio_set_direction((gpio_num_t)PIN_NW, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)PIN_NW, ! _level);
gpio_set_direction((gpio_num_t)PIN_CW, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)PIN_CW, ! _level);

  ledc_timer_config_t ledc_timer_cfg = {
      .speed_mode = SPEED_MODE,
      {.duty_resolution = LEDC_TIMER_8_BIT},
      .timer_num = BAND_TIMER_NUM,
      .freq_hz = 5000};

  if (ledc_timer_config(&ledc_timer_cfg) != ESP_OK)
  {
   // esp_timer_delete(_timer);
  //  _timer = NULL;
    Serial.println("Error creating ldec");
    //ESP_LOGE(TAG, "Error configuring LEDC timer!\r\n");
    return;
  }
  
   //ledcAttachPin(PIN_CW, CANNEL_CW);
   // ledcAttachPin(PIN_NW, CANNEL_NW);
   // ledcAttachPin(PIN_WW, CANNEL_WW);
   //ledc_set_pin(PIN_CW,SPEED_MODE,CANNEL_CW);
  // ledc_set_pin(PIN_NW,SPEED_MODE,CANNEL_NW);
  // ledc_set_pin(PIN_WW,SPEED_MODE,CANNEL_WW);

   ledc_channel_config_t ledc_channel = {
        .gpio_num       = PIN_CW,
        .speed_mode     = SPEED_MODE,
        .channel        = CANNEL_CW,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = BAND_TIMER_NUM,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
    ledc_channel.gpio_num=PIN_NW;
    ledc_channel.channel=CANNEL_NW;
    ledc_channel_config(&ledc_channel);
    ledc_channel.gpio_num=PIN_WW;
    ledc_channel.channel=CANNEL_WW;
    ledc_channel_config(&ledc_channel);


    event_t ev;

    ev.state= MEM_EVENT;
    ev.button=101;
    xQueueSend(que,&ev,portMAX_DELAY);
    delay(100);
    ev.button=102;
    xQueueSend(que,&ev,portMAX_DELAY);
    delay(100);
    ev.button=103;
    xQueueSend(que,&ev,portMAX_DELAY);

}

void BANDTask::setOne(uint8_t cannel, uint8_t value, bool sav){
       switch (cannel){
          case 0:
           ledc_set_duty(SPEED_MODE, CANNEL_CW, _level?value:255-value);
           ledc_update_duty(SPEED_MODE, CANNEL_CW);
          break;
          case 1:
           ledc_set_duty(SPEED_MODE, CANNEL_NW, _level?value:255-value);
           ledc_update_duty(SPEED_MODE, CANNEL_NW);
          break;
          case 2:
           ledc_set_duty(SPEED_MODE, CANNEL_WW, _level?value:255-value);
           ledc_update_duty(SPEED_MODE, CANNEL_WW);
          break;
        }
        
        if (sav){
        event_t ev;
        ev.state=MEM_EVENT;
        ev.button=201+cannel;
        ev.count=value;
        String s="CCannel " + String(cannel==0?"COOL":cannel==1?"NORMAL":"WARM") + "*set to "+String(value)+"**";
        xMessageBufferSend(disp_mess,s.c_str(),s.length(),portMAX_DELAY);
        xQueueSend(que,&ev,portMAX_DELAY);
        }
       } 

void BANDTask::loop()
{
  uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    //uint8_t comm,val;
    //uint16_t data;
    notify_t notify;

    //readPacket(command,&comm,&val,&data);
    memcpy(&notify,&command,sizeof(notify));
    switch (notify.title)
    {
    case 1://set one channel
      setOne(notify.packet.var,notify.packet.value);
      break;
    case 2://all on
       setOne(0,255);
       setOne(1,255);
       setOne(2,255);
      break;
    case 3://all off
      setOne(0,0);
      setOne(1,0);
      setOne(2,0);
      break;
    case 4://all half
      setOne(0,128);
      setOne(1,128);
      setOne(2,128);
      break;
    case 5://all low
      setOne(0,64);
      setOne(1,64);
      setOne(2,64);
      break;
    case 6://cool
      setOne(0,255);
      setOne(1,64);
      setOne(2,64);
      break;
    case 7://normal
      setOne(0,64);
      setOne(1,255);
      setOne(2,64);
      break;
    case 8://warm
      setOne(0,64);
      setOne(1,64);
      setOne(2,255);
      break;
    case 9://low warm
      setOne(0,0);
      setOne(1,0);
      setOne(2,128);
      break;
    case 10://set one channel
      setOne(notify.packet.var,notify.packet.value,false);
      break;
  }
  }
}

void BANDTask::cleanup()
{
 
    ledc_stop(SPEED_MODE, CANNEL_CW, !_level);
    ledc_stop(SPEED_MODE, CANNEL_NW, !_level);
    ledc_stop(SPEED_MODE, CANNEL_WW, !_level);
    gpio_reset_pin((gpio_num_t)PIN_CW);
    gpio_reset_pin((gpio_num_t)PIN_NW);
    gpio_reset_pin((gpio_num_t)PIN_WW);
 
}

