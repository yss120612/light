#ifndef _LED_h
#define _LED_h

#include <esp_timer.h>
#include <driver/ledc.h>
#include <HardwareSerial.h>

const ledc_timer_t TIMER_NUM = LEDC_TIMER_3;
const ledc_mode_t SPEED_MODE = LEDC_LOW_SPEED_MODE;
enum blinkmode_t { BLINK_OFF, BLINK_ON, BLINK_TOGGLE, BLINK_05HZ, BLINK_1HZ, BLINK_2HZ, BLINK_4HZ, BLINK_FADEIN, BLINK_FADEOUT, BLINK_FADEINOUT, BLINK_SUNRAISE,BLINK_SUNSET };


class Led {
public:
   Led(uint8_t pin, bool level, ledc_channel_t channel, esp_timer_handle_t timer);
  ~Led();

  operator bool() {
    return _timer != NULL;
  }
  void operator=(blinkmode_t value) {
    setMode(value);
  }

  int32_t getValue() const {
    return _value;
  }

  int32_t absValue() const {
    return _value<0?-_value:_value;
  }

  void setValue(int32_t value) {
    _value=value;
  }

  int32_t incValue() {return ++_value;}
  int32_t decValue() {return --_value;}

  blinkmode_t getMode() const {
    return _mode;
  }

  bool getLevel() const{
    return _level;
  }

  uint8_t getPin() const{
    return _pin;
  }
  
  void setMode(blinkmode_t mode);

  ledc_channel_t getChannel() const {
    return _channel;
  }

protected:
    uint8_t _pin;
    bool _level;
    blinkmode_t _mode;
  
    ledc_channel_t _channel;
    volatile int32_t _value;
    esp_timer_handle_t _timer;
  
};
#endif