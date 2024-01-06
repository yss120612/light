#ifndef _SETTINGS_h
#define _SETTINGS_h
#include <Arduino.h>
#include <GlobalSettings.h>
//#define DEBUGG


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

const uint8_t leds_pins[]={GPIO_NUM_23, GPIO_NUM_33, GPIO_NUM_15, GPIO_NUM_2};
const uint8_t relays_pins[] = {GPIO_NUM_32, GPIO_NUM_5, GPIO_NUM_19, 0};
const uint8_t IR_PIN = GPIO_NUM_34; // pin for IR receiver
const uint8_t IR_DEVICE = 162;
const uint8_t BMP280_ADDRESS=0x67;//BMP280 ADDRESS in I2C
const gpio_num_t ENCBTN = GPIO_NUM_13;//ENCODER BUTTON
const gpio_num_t ENCS1 = GPIO_NUM_NC;//ENCODER A
const gpio_num_t ENCS2 = GPIO_NUM_NC;//ENCODER B
const uint8_t AT24C32_ADDRESS = 0x57;
const uint8_t AT24C32_OFFSET = 0x80;

#define VERSION 6

#define PULT_1      1
#define PULT_2      2
#define PULT_3      3
#define PULT_4      4
#define PULT_5      5
#define PULT_6      6
#define PULT_7      7
#define PULT_8      8
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



#define LEDS_COUNT 3
#define RELAYS_COUNT 4

struct __attribute__((__packed__)) SystemState_t
{
    uint8_t version : 8;
    alarm_t alr[ALARMS_COUNT];
    relState_t relays[RELAYS_COUNT];
    led_state_t leds[LEDS_COUNT];

    uint8_t crc;
	
};

static void reset_default(SystemState_t * ss){
		ss->version=VERSION;
        uint8_t i;
		for (i=0;i<ALARMS_COUNT;i++){
			ss->alr[i].action=i;
			ss->alr[i].active=false;
			ss->alr[i].hour=0;
			ss->alr[i].minute=0;
			ss->alr[i].wday=0;
			ss->alr[i].period=ONCE_ALARM;
		}
        for (i=0;i<RELAYS_COUNT;i++){
			ss->relays[i].type=RELTYPE_SWICH;
            ss->relays[i].armed=false;
            ss->relays[i].ison=false;
            ss->relays[i].level=HIGH;
		}
        for (i=0;i<LEDS_COUNT;i++){
			ss->leds[i].state=BLINK_OFF;
            ss->leds[i].value=0;
		}

		ss->crc=crc8((uint8_t*)ss, sizeof(ss));
	}












static uint8_t process_notify(SystemState_t * ss, event_t * event, notify_t nt){
	uint8_t i;
	
switch (nt.title)
	{
		case MEM_ASK_00://timers
		case MEM_ASK_01:
		case MEM_ASK_02:
		case MEM_ASK_03:
		case MEM_ASK_04:
		case MEM_ASK_05:
		case MEM_ASK_06:
		case MEM_ASK_07:
		case MEM_ASK_08:
		case MEM_ASK_09:
			event->state=MEM_EVENT;
			event->button=nt.title-50;//MEM_READ_XX
			event->alarm=ss->alr[nt.title-MEM_ASK_00];
		break;
        case MEM_ASK_10://relay 1
		case MEM_ASK_11://relay 2
        case MEM_ASK_12://relay 3
        case MEM_ASK_13://relay 4
			event->state=MEM_EVENT;
			event->button=nt.title-50;//MEM_READ_XX
			event->data=rel_state2uint32(ss->relays[nt.title-MEM_READ_10]);
		
		case MEM_ASK_14://led1
        case MEM_ASK_15://led2
        case MEM_ASK_16://led3
			event->state=MEM_EVENT;
			event->button=nt.title-50;//MEM_READ_XX;
			event->data=led_state2uint32(ss->leds[nt.title-MEM_READ_14]);
		break;
		
		case MEM_SAVE_00:
		case MEM_SAVE_01:
		case MEM_SAVE_02:
		case MEM_SAVE_03:
		case MEM_SAVE_04:
		case MEM_SAVE_05:
		case MEM_SAVE_06:
		case MEM_SAVE_07:
		case MEM_SAVE_08:
		case MEM_SAVE_09:
			ss->alr[nt.title-MEM_SAVE_00]=nt.alarm;
		break;
		case MEM_SAVE_10:
        case MEM_SAVE_11:
        case MEM_SAVE_12:
        case MEM_SAVE_13:
			ss->relays[nt.title-MEM_SAVE_10]=uint322rel_state(nt.packet.value);
		break;
		case MEM_SAVE_14:
        case MEM_SAVE_15:
        case MEM_SAVE_16:
			ss->leds[nt.title-MEM_SAVE_10]=uint322led_state(nt.packet.value);
		break;
    }
	if (nt.title<MEM_ASK_00) return 1;//MEM_READ
	if (nt.title<MEM_SAVE_00) return 2;//MEM_ASK
	return 3;
}
const uint16_t SSTATE_LENGTH = sizeof(SystemState_t);
#endif