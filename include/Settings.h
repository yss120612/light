#ifndef _SETTINGS_h
#define _SETTINGS_h

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
#define CANNEL_CW   0
#define CANNEL_NW   1
#define CANNEL_WW   2


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

#endif