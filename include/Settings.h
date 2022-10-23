#ifndef _SETTINGS_h
#define _SETTINGS_h
#include <driver/ledc.h>

#define LED     2
#define IRPIN   4
#define RELAY1  5
#define RELAY2  18
#define RELAY3  19
#define RELAY4  17

#define BUTTON_1 13

// #define PULT_1      0x58A7EA15
// #define PULT_2      0x58A7827D
// #define PULT_3      0x58A7629D
// #define PULT_4      0x58A7906F
// #define PULT_POWER  0x58A728D7

#define IR_DEVICE 162
//#define DISPLAY_1   67


#define PULT_1      1
#define PULT_2      2
#define PULT_3      3
#define PULT_4      4
#define PULT_POWER  74
#define PULT_SOUND  69
#define PULT_VOLUP  14
#define PULT_VOLDOWN  15
#define PULT_FASTBACK 110
#define PULT_FASTFORWARD  30
#define PULT_NEXT 82
#define PULT_PREV  83
#define PULT_SLOW  17
#define PULT_ZOOM  24
#define PULT_STOP  22
#define PULT_PAUSE  16
#define PULT_INFO  67

#define WEB_CANNEL_CW   100
#define WEB_CANNEL_NW   101
#define WEB_CANNEL_WW   102

#define PRESS_DURATION 300//держим реле (имитация нажатия) 
#define PERIOD 2500
#define CHECKPERIOD 100
#define CHECKWIFI 300000

#define RELTYPE_SWICH 0
#define RELTYPE_BUTTON 1

//PWM
#define RESOLUTION  8
#define FREQ        5000
#define CANNEL_CW   LEDC_CHANNEL_0
#define CANNEL_NW   LEDC_CHANNEL_1
#define CANNEL_WW   LEDC_CHANNEL_2


#define PIN_CW   23
#define PIN_NW   16
#define PIN_WW   15

#define CLICK_TIME 20           // 20 ms.
#define LONGCLICK_TIME 1000     // 1 sec.
#define DBLCLICK_TIME 500       // 0.5 sec.

#define  AT24C32_ADDRESS 0x57
#define  EEPROM_PAGE_SIZE  32
#define  EEPROM_WORK_SIZE  EEPROM_PAGE_SIZE / 2
#define  EEPROM_WRITE_TIMEOUT  10

#define  TIME_OFFSET 8

#define LONG_TIME  60000*60*24 //one per day
#define SHORT_TIME  60000*5 //one per 5 min

#define DISPLAY_ON_TIME 1000*5

#define LED_TIMER_NUM LEDC_TIMER_3
#define BAND_TIMER_NUM LEDC_TIMER_2

#define SPEED_MODE LEDC_LOW_SPEED_MODE

#define CANNEL_CW LEDC_CHANNEL_0
#define CANNEL_NW LEDC_CHANNEL_1
#define CANNEL_WW LEDC_CHANNEL_2


const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};

enum blinkmode_t { BLINK_OFF, BLINK_ON, BLINK_TOGGLE, BLINK_05HZ, BLINK_1HZ, BLINK_2HZ, BLINK_4HZ, BLINK_FADEIN, BLINK_FADEOUT, BLINK_FADEINOUT, BLINK_SUNRAISE,BLINK_SUNSET };

const uint8_t rpins[]={RELAY1,RELAY2,RELAY3,RELAY4};

enum flags_t : uint8_t { FLAG_WIFI = 1, FLAG_MQTT = 2 };

void static readPacket(uint32_t container, uint8_t &btn, uint8_t &value, uint16_t &data){
    btn   = (container >> 24) & 0x000000FF;
    value = (container >> 16) & 0x000000FF;
    data  = container & 0x0000FFFF;
}
 static uint32_t makePacket(uint8_t btn, uint8_t value, uint16_t data){
    uint32_t container;
    container = (btn << 24) & 0xFF000000 | (value<<16) & 0x00FF0000 | data & 0x0000FFFF;
    return container;
}

#endif