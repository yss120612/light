#ifndef __STATE_h
#define __STATE_h
#define VERSION 9

struct __attribute__((__packed__)) SystemState_t
{
    uint8_t version : (uint8_t)VERSION;
    alarm_t alr[ALARMS_COUNT];
    relState_t relays[RELAYS_COUNT];
    led_state_t leds[LEDS_COUNT];
    uint8_t crc;
};

class State{
    public:
    SystemState_t st;
};

#endif