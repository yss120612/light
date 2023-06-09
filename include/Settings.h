#ifndef _SETTINGS_h
#define _SETTINGS_h
//#include <driver/ledc.h>
#define DEBUGG


//#define LED     2
//#define IRPIN   4
// #define RELAY1  17
// #define RELAY2  5
// #define RELAY3  19
// //#define RELAY4  18
// #define RELAY4  0

//#define BUTTON_1 13

// #define PULT_1      0x58A7EA15
// #define PULT_2      0x58A7827D
// #define PULT_3      0x58A7629D
// #define PULT_4      0x58A7906F
// #define PULT_POWER  0x58A728D7

//#define DISPLAY_1   67

const uint8_t leds_pins[]={GPIO_NUM_23, GPIO_NUM_16, GPIO_NUM_15, GPIO_NUM_2};
const uint8_t relays_pins[] = {GPIO_NUM_17, GPIO_NUM_5, GPIO_NUM_19, 0};
const uint8_t IR_PIN = GPIO_NUM_4; // pin for IR receiver
const uint8_t IR_DEVICE = 162;
const uint8_t BMP280_ADDRESS=0x67;//BMP280 ADDRESS in I2C
const int8_t ENCBTN = GPIO_NUM_13;//ENCODER BUTTON
const int8_t ENCS1 = GPIO_NUM_27;//ENCODER A
const int8_t ENCS2 = GPIO_NUM_26;//ENCODER B
const uint8_t AT24C32_ADDRESS = 0x57;
const uint8_t AT24C32_OFFSET = 0x100;

#define VERSION 5

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


// #define WEB_CANNEL_CW   100
// #define WEB_CANNEL_NW   101
// #define WEB_CANNEL_WW   102




//#define DISP_MESSAGE_LENGTH 120
#endif