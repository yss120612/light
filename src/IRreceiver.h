#ifndef _IRRECEIVER_h
#define _IRRECEIVER_h

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>


class IRreceiver
{
    public:
    IRreceiver(uint16_t port);
    ~IRreceiver();
    uint32_t checkIR(unsigned long ms);
    void enable();
    void disable();
    void sleep_sometime(unsigned long time=24000);
    uint32_t getCommand();
    private:
    IRrecv * irrecv;
    uint16_t _port;
    decode_results dres;
    uint32_t last_command;
    unsigned long time_lc;
    unsigned long sleeping_time;
};

#endif