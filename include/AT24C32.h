#ifndef _AT24C32_h
#define _AT24C32_h

#include "arduino.h"
#include "Settings.h"

#include <Wire.h>

class AT24C32 {
public:
	AT24C32(){};
	//void init(bool fast = false);
	//void init(int8_t pinSDA, int8_t pinSCL, bool fast = false);
	bool begin(uint8_t address = AT24C32_ADDRESS);
	//uint8_t read(uint16_t index);
	void read(uint16_t index, uint8_t* buf, uint16_t len);
	//void write(uint16_t index, uint8_t data);
	void write(uint16_t index, const uint8_t* buf, uint16_t len);
	template<typename T> T & get(uint16_t index, T& t) {
		read(index, (uint8_t*)&t, sizeof(T));
		return t;
	}
	template<typename T> const T& put(uint16_t index, const T& t) {
		write(index, (const uint8_t*)&t, sizeof(T));
		return t;
	}

private:
	uint8_t _address;
};

#endif

