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
#define PULT_POWER  28
#define PULT_SOUND  18
#define PULT_VOLUP  10
#define PULT_VOLDOWN  11
#define PULT_FASTBACK 15
#define PULT_FASTFORWARD  14
#define PULT_NEXT 16
#define PULT_PREV  17
#define PULT_PLAY  23 
#define PULT_STOP  22
#define PULT_PAUSE  24


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
#define  AT24C32_OFFSET 0x78
//#define  AT24C32_ADDRESS 0x78
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

const uint8_t pins[]={LED,0,0,0};
const ledc_channel_t channels[]={LEDC_CHANNEL_0,LEDC_CHANNEL_1,LEDC_CHANNEL_2,LEDC_CHANNEL_3};
enum buttonstate_t : uint8_t {  
    NONE_EVENT,
    BTN_CLICK,
    BTN_LONGCLICK,
    PULT_BUTTON,
    WEB_EVENT,
    MEM_EVENT,
    DISP_EVENT,
    LED_EVENT,
    ENCODER_EVENT,
    RTC_EVENT
 };
enum blinkmode_t { BLINK_OFF, BLINK_ON, BLINK_TOGGLE, BLINK_05HZ, BLINK_1HZ, BLINK_2HZ, BLINK_4HZ, BLINK_FADEIN, BLINK_FADEOUT, BLINK_FADEINOUT, BLINK_SUNRAISE,BLINK_SUNSET };

const uint8_t rpins[]={RELAY1,RELAY2,RELAY3,RELAY4};
const uint8_t bpins[]={BUTTON_1,0,0,0};
const bool blevels[]={true,false,false,false};
//static const char * dayofweek[] ={"Воскресенье","Понедельник","Вторник","Среда","Четверг","Пятница","Суббота"};
static const char dayofweek[] = "SunMonTueWedThuFriSat";


enum flags_t : uint8_t { FLAG_WIFI = 1, FLAG_MQTT = 2 };

void static readPacket(uint32_t container, uint8_t * btn, uint8_t * value, uint16_t * data){
    *btn   = (container >> 24) & 0x000000FF;
    *value = (container >> 16) & 0x000000FF;
    *data  = container & 0x0000FFFF;
}
 static uint32_t makePacket(uint8_t btn, uint8_t value, uint16_t data){
    uint32_t container;
    container = (btn << 24) & 0xFF000000 | (value<<16) & 0x00FF0000 | data & 0x0000FFFF;
    return container;
}

enum period_t : uint8_t
{
    NONE_ALARM,
    ONCE_ALARM,
    EVERYHOUR_ALARM,
    EVERYDAY_ALARM,
    WDAY_ALARM,
    HDAY_ALARM,
    WD7_ALARM,
    WD1_ALARM,
    WD2_ALARM,
    WD3_ALARM,
    WD4_ALARM,
    WD5_ALARM,
    WD6_ALARM
};


#define ALARMS_COUNT 10
const uint16_t WEEK = 10080; // minutes in week
const uint16_t DAY = 1440;   // minutes in day

struct __attribute__((__packed__)) alarm_t
{
    bool active : 1;
    uint8_t hour : 5;
    uint8_t minute : 6;
    uint8_t action : 4;
    uint8_t wday : 3;
    period_t period : 5;
};

struct __attribute__((__packed__)) bool1_t
{
  bool ison:1;
   bool anyth :1;
};

struct __attribute__((__packed__)) SystemState_t
{
    uint8_t version : 8;
    bool rel[4];
    uint8_t br[3];
    alarm_t alr[ALARMS_COUNT];
    uint8_t crc;
};

const uint16_t SSTATE_LENGTH = sizeof(SystemState_t);
// const uint16_t ALARMS_OFFSET=512;

static void getNext(alarm_t &at)
{
    switch (at.period)
    {
    case ONCE_ALARM:
        at.active = false;
        break;
    case WDAY_ALARM:
        if (at.wday >= 5)
            at.wday = 1;
        else
            at.wday++;
        break;
    case HDAY_ALARM:
        if (at.wday == 6)
            at.wday = 0;
        else
            at.wday = 6;
        break;
    case EVERYDAY_ALARM:
        if (at.wday == 6)
            at.wday = 0;
        else
            at.wday++;
        break;
    case EVERYHOUR_ALARM:
        if (at.hour >= 23)
            at.hour = 0;
        else
            at.hour++;
        break;
    case WD1_ALARM:
    case WD2_ALARM:
    case WD3_ALARM:
    case WD4_ALARM:
    case WD5_ALARM:
    case WD6_ALARM:
    case WD7_ALARM:
        break;
    }
}

static std::string printAlarm(alarm_t at)
{
    std::string per = at.active ? "+" : "-";

    switch (at.period)
    {
    case ONCE_ALARM:
        per += " 1t.";
        break;
    case WDAY_ALARM:
        per += " Wd.";
        break;
    case HDAY_ALARM:
        per += " Hd.";
        break;
    case EVERYDAY_ALARM:
        per += " 1d.";
        break;
    case EVERYHOUR_ALARM:
        per += " 1h.";
        break;
    case WD7_ALARM:
        per += " Vs.";
        break;
    case WD1_ALARM:
        per += " Pn.";
        break;
    case WD2_ALARM:
        per += " Vt.";
        break;
    case WD3_ALARM:
        per += " Sr.";
        break;
    case WD4_ALARM:
        per += " Ct.";
        break;
    case WD5_ALARM:
        per += " Pt.";
        break;
    case WD6_ALARM:
        per += " Sb.";
        break;
    }
    char buf[30];
    uint8_t res = snprintf(buf, sizeof(buf), "%d %02d:%02d %s (%d)\n", at.action, at.hour, at.minute, per.c_str(), at.wday);
    std::string str = "error!";
    if (res >= 0 && res < sizeof(buf))
        str = buf;
    return str;
}

struct event_t
{
    buttonstate_t state;
    uint16_t button;
    uint8_t count;
    // int8_t type;
    union
    {
        uint32_t data;
        alarm_t alarm;
    };

    //  volatile long wait_time;
};

struct __attribute__((__packed__)) notify_packet_t
{
    uint8_t var : 8;
    uint16_t value : 16;
};

struct notify_t
{
    uint8_t title;
    union
    {
        notify_packet_t packet;
        alarm_t alarm;
    };
};



static uint8_t crc8(uint8_t *buffer, uint16_t size) {
  uint8_t crc = 0;
  for (uint16_t i = 0; i < size; i++) {
    uint8_t data = buffer[i];
    for (uint8_t j = 8; j > 0; j--) {
      crc = ((crc ^ data) & 1) ? (crc >> 1) ^ 0x8C : (crc >> 1);
      data >>= 1;
    }
  }
  return crc;
}


#endif