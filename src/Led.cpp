#include <esp_log.h>
#include "Led.h"

//static const char TAG[] = "Blinker";



static int8_t abs8(int8_t value) {
  if (value < 0)
    return -value;
  return value;
}


Led::Led(uint8_t pin, bool level, ledc_channel_t channel, esp_timer_handle_t timer){
  _mode = BLINK_OFF;
  _timer = timer;
  _level=level;
  _channel = channel;
  _value = 0;
  _pin=pin;
  pinMode(pin,OUTPUT);
   gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
   gpio_set_level((gpio_num_t)pin, ! level);
}

Led::~Led() {
}

void Led::setMode(blinkmode_t mode) {
 // if (_mode==mode && mode!=BLINK_TOGGLE) return;

  if (_mode >= BLINK_FADEIN) {
          ledc_stop(SPEED_MODE, _channel, ! _level);
          gpio_set_direction((gpio_num_t)_pin, GPIO_MODE_OUTPUT);
      } 
  if (_timer && esp_timer_is_active(_timer)) esp_timer_stop(_timer);
 

  uint32_t period;

  switch (mode)
    {
    case BLINK_OFF:
    case BLINK_ON:
         gpio_set_level((gpio_num_t)_pin, mode == BLINK_ON ? _level : ! _level);
        break;
    case BLINK_TOGGLE:
        gpio_set_level((gpio_num_t)_pin, ! gpio_get_level((gpio_num_t)_pin));
       break;
    case BLINK_05HZ:
    case BLINK_1HZ:
    case BLINK_2HZ:
    case BLINK_4HZ:
          break;
    case BLINK_FADEIN:
    case BLINK_FADEOUT:
    case BLINK_FADEINOUT:
    case BLINK_SUNRAISE:
    case BLINK_SUNSET:
        ledc_channel_config_t ledc_channel_cfg1;
          ledc_channel_cfg1.gpio_num=_pin;
          ledc_channel_cfg1.speed_mode=SPEED_MODE;
          ledc_channel_cfg1.intr_type=LEDC_INTR_DISABLE;
          ledc_channel_cfg1.channel=_channel;
          ledc_channel_cfg1.timer_sel=TIMER_NUM;
          ledc_channel_cfg1.duty= _level ? 0 : 255;
        
        if (ledc_channel_config(&ledc_channel_cfg1) != ESP_OK) {
          return;
         }
        break;
  
    }

  _mode=mode;
  _value = 0;
}

