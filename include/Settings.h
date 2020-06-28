#ifndef _SETTINGS_h
#define _SETTINGS_h

#define LED     2
#define IRPIN   4
#define RELAY1  5
#define RELAY2  18
#define RELAY3  19
#define RELAY4  21

// #define PULT_1      0x58A7EA15
// #define PULT_2      0x58A7827D
// #define PULT_3      0x58A7629D
// #define PULT_4      0x58A7906F
// #define PULT_POWER  0x58A728D7

#define IR_DEVICE 26
#define PULT_1      87
#define PULT_2      65
#define PULT_3      70
#define PULT_4      9
#define PULT_POWER  20
#define PULT_SOUND  69
#define PULT_VOLUP  68
#define PULT_VOLDOWN  24
#define PULT_FASTBACK 10
#define PULT_FASTFORWARD  30
#define PULT_NEXT 82
#define PULT_PREV  83
#define PULT_SLOW  64
#define PULT_ZOOM  15
#define PULT_STOP  14
#define PULT_PAUSE  1

#define PRESS_DURATION 300//держим реле (имитация нажатия) 
#define PERIOD 1000
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
#define PIN_NW   22
#define PIN_WW   15



#endif