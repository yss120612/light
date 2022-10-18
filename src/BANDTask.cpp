#include "BANDTask.h"

void BANDTask::setup()
{

  esp_timer_create_args_t timer_cfg;
  if (esp_timer_create(&timer_cfg, &_timer) != ESP_OK)
  {
    _timer = NULL;
     return;
  }

  ledc_timer_config_t ledc_timer_cfg = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      {.duty_resolution = LEDC_TIMER_8_BIT},
      .timer_num = BAND_TIMER_NUM,
      .freq_hz = 1000};

  if (ledc_timer_config(&ledc_timer_cfg) != ESP_OK)
  {
    esp_timer_delete(_timer);
    _timer = NULL;
    //ESP_LOGE(TAG, "Error configuring LEDC timer!\r\n");
    return;
  }
gpio_set_direction((gpio_num_t)PIN_WW, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)PIN_WW, ! _level);
gpio_set_direction((gpio_num_t)PIN_NW, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)PIN_NW, ! _level);
gpio_set_direction((gpio_num_t)PIN_CW, GPIO_MODE_OUTPUT);
gpio_set_level((gpio_num_t)PIN_CW, ! _level);
  
  
}

void BANDTask::setOne(uint8_t cannel, uint8_t value){

ledc_channel_config_t ledc_channel_cfg1;
          ledc_channel_cfg1.speed_mode=SPEED_MODE;
          ledc_channel_cfg1.intr_type=LEDC_INTR_DISABLE;
          ledc_channel_cfg1.timer_sel=LED_TIMER_NUM;
          ledc_channel_cfg1.duty= _level ? value : 255-value;
        switch (cannel){
          case 0:
          ledc_channel_cfg1.channel=CANNEL_CW;  
          ledc_channel_cfg1.gpio_num=PIN_CW;
          break;
          case 1:
          ledc_channel_cfg1.channel=CANNEL_NW;  
          ledc_channel_cfg1.gpio_num=PIN_NW;
          break;
          case 2:
          ledc_channel_cfg1.channel=CANNEL_WW;  
          ledc_channel_cfg1.gpio_num=PIN_WW;
          break;
        }
        
        if (ledc_channel_config(&ledc_channel_cfg1) != ESP_OK) {
          return;
         }
        


} 

void BANDTask::loop()
{
  uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    uint8_t comm=command>>24 & 0x000000FF;
    uint8_t l=command>>16 & 0x000000FF;
    uint16_t h=command & 0x0000FFFF;

    switch (comm)
    {
    case 1://set one channel
      setOne(l,h);
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
    case 5://cool
      setOne(0,255);
      setOne(1,64);
      setOne(2,64);
      break;
    case 6://normal
      setOne(0,64);
      setOne(1,255);
      setOne(2,64);
      break;
    case 7://warm
      setOne(0,64);
      setOne(1,64);
      setOne(2,255);
      break;
    case 8://low warm
      setOne(0,0);
      setOne(1,0);
      setOne(2,128);
      break;
   
  }
  }
}

void BANDTask::cleanup()
{
  if (_timer)
  {
    esp_timer_stop(_timer);
    esp_timer_delete(_timer);
    
    ledc_stop(SPEED_MODE, CANNEL_CW, !_level);
    ledc_stop(SPEED_MODE, CANNEL_NW, !_level);
    ledc_stop(SPEED_MODE, CANNEL_WW, !_level);
    gpio_reset_pin((gpio_num_t)PIN_CW);
    gpio_reset_pin((gpio_num_t)PIN_NW);
    gpio_reset_pin((gpio_num_t)PIN_WW);
  }
}

