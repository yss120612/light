#include "LEDTask.h"

void LEDTask::setup()
{

  esp_timer_create_args_t timer_cfg = {
      .callback = (esp_timer_cb_t)&LEDTask::timerCallback,
      .arg = this};

  if (esp_timer_create(&timer_cfg, &_timer) != ESP_OK)
  {
    _timer = NULL;
    //ESP_LOGE(TAG, "Error creating blinker timer!\r\n");
    return;
  }

  ledc_timer_config_t ledc_timer_cfg = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      {.duty_resolution = LEDC_TIMER_8_BIT},
      .timer_num = TIMER_NUM,
      .freq_hz = 1000};

  if (ledc_timer_config(&ledc_timer_cfg) != ESP_OK)
  {
    esp_timer_delete(_timer);
    _timer = NULL;
    //ESP_LOGE(TAG, "Error configuring LEDC timer!\r\n");
    return;
  }

  // ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};
  for (uint8_t i = 0; i < 4; i++)
    if (pins[i] > 0)
    {
      led[i] = new Led(pins[i], _level, channels[i], _timer);
    }
    else
      led[i] = NULL;
  last_time = 0;

  setLedMode(0, BLINK_05HZ);
  setLedMode(1, BLINK_OFF);
}

 void LEDTask::setLedMode(uint8_t ledN, blinkmode_t bm){
        if (ledN<4 && led[ledN]) 
        led[ledN]->setMode(bm);
        bool need_timer=false;
        for (int8_t i=0;i<4;i++) 
        {
            if (!led[i]) break;
            need_timer=led[i]->getMode()>BLINK_TOGGLE;
            if (need_timer) break;
        }
        if (need_timer) 
      
   {
   if (esp_timer_start_periodic(_timer, 50000) != ESP_OK) {
           return;
         }
   }
    }

void LEDTask::loop()
{
  uint32_t command;
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    switch (command)
    {
    case 17:
      setLedMode(1, BLINK_FADEINOUT);
      break;
    case 18:
      setLedMode(1, BLINK_FADEIN);
      break;
    case 19:
      setLedMode(1, BLINK_FADEOUT);
      break;
    case 20:
      setLedMode(1, BLINK_1HZ);
      break;
    case 21:
      setLedMode(0, BLINK_OFF);
      setLedMode(1, BLINK_OFF);
      break;
    case 23:
      setLedMode(0, BLINK_05HZ);
      setLedMode(1, BLINK_OFF);
      break;
    case 24:
      setLedMode(0, BLINK_2HZ);
      setLedMode(1, BLINK_OFF);
      break;
    case 25:
      setLedMode(0, BLINK_4HZ);
      setLedMode(1, BLINK_OFF);
      break;
    case 33:
      setLedMode(0, BLINK_4HZ);
    break;

    case 111:
      //setLedMode(0, BLINK_4HZ);
      setLedMode(1, BLINK_4HZ);
      break;
    
    case 112:
      //setLedMode(0, BLINK_4HZ);
      setLedMode(1, BLINK_FADEINOUT);
      break;
    case 113:
      //setLedMode(0, BLINK_4HZ);
      setLedMode(1, BLINK_OFF);
      break;
  }
  }
}

void LEDTask::cleanup()
{
  if (_timer)
  {
    esp_timer_stop(_timer);
    esp_timer_delete(_timer);
    for (uint8_t i = 0; i < 4; i++)
    {
      if (pins[i] <= 0)
        break;
      if (led[i] && led[i]->getMode() >= BLINK_FADEIN)
      {
        ledc_stop(SPEED_MODE, channels[i], !_level);
      }
      gpio_reset_pin((gpio_num_t)pins[i]);
      delete (led[i]);
    }
  }
}

void LEDTask::timerCallback()
{

  const uint8_t FADES_SIZE = 32;
  // const uint8_t FADES[FADES_SIZE] = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 192, 224, 240, 248, 252, 254, 255 };
  const uint16_t SUN_MULT = 100;
  const uint8_t FADES[FADES_SIZE] = {0, 8, 16, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 255};

  for (uint8_t i = 0; i < 4; i++)
  {
    if (!led[i])
      continue;
    if (led[i]->getMode() < BLINK_05HZ)
      continue;
    if (led[i]->getMode() <= BLINK_4HZ)
    {
      if (led[i]->getValue() == 0)
      {
        gpio_set_level((gpio_num_t)led[i]->getPin(), led[i]->getLevel());
      }
      else if (led[i]->getValue() == 1)
      {
        gpio_set_level((gpio_num_t)led[i]->getPin(), !led[i]->getLevel());
      }
      if (led[i]->incValue() >= (led[i]->getMode() == BLINK_05HZ ? 40 : led[i]->getMode() == BLINK_1HZ ? 20
                                                                    : led[i]->getMode() == BLINK_2HZ   ? 10
                                                                                                       : 2))
        led[i]->setValue(0);
    }
    else if (led[i]->getMode() < BLINK_SUNRAISE)
    { // _mode >= BLINK_FADEIN

      // ledc_set_duty(SPEED_MODE, led[i]->getChannel(), led[i]->getLevel() ? FADES[led[i]->absValue()] : 255 - FADES[led[i]->absValue()]);
      ledc_set_duty(SPEED_MODE, led[i]->getChannel(), FADES[led[i]->absValue()]);
      ledc_update_duty(SPEED_MODE, led[i]->getChannel());
      if (led[i]->getMode() == BLINK_FADEIN)
      {

        if (led[i]->incValue() >= FADES_SIZE)
          led[i]->setValue(0);
      }
      else if (led[i]->getMode() == BLINK_FADEOUT)
      {
        if (led[i]->decValue() < 0)
          led[i]->setValue(FADES_SIZE - 1);
      }
      else if (led[i]->getMode() == BLINK_FADEINOUT)
      {
        if (led[i]->incValue() >= FADES_SIZE)
          led[i]->setValue(-(FADES_SIZE - 2));
      }
    }
    else
    {
      bool change_light;
      int8_t bright = led[i]->getValue() / SUN_MULT;
      change_light = led[i]->getValue() % SUN_MULT == 0;

      if (led[i]->getMode() == BLINK_SUNRAISE)
      {
        if (bright > FADES_SIZE - 1)
        {
          setLedMode(i, BLINK_ON);
          return;
        }
      }
      else if (led[i]->getMode() == BLINK_SUNSET)
      {
        if (bright > FADES_SIZE - 1)
        {
          setLedMode(i, BLINK_OFF);
          return;
        }
      }
      led[i]->incValue();
      if (change_light)
      {
        int8_t index = led[i]->getMode() == BLINK_SUNRAISE ? bright : FADES_SIZE - 1 - bright;
        // int16_t val=led[i]->getLevel()?FADES[index] : (FADES[index]>0?256:255) - FADES[index];
        int16_t val = FADES[index];
        // ledc_set_duty(SPEED_MODE, led[i]->getChannel(), led[i]->getLevel() ? FADES[index] : 255 - FADES[index]);

        ledc_set_duty(SPEED_MODE, led[i]->getChannel(), val);
        ledc_update_duty(SPEED_MODE, led[i]->getChannel());
        Serial.print("val=");
        Serial.println(val);
        Serial.print("index=");
        Serial.println(index);
      }
    }
  }
}
