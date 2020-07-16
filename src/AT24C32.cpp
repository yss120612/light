#include "AT24C32.h"


bool AT24C32::begin(uint8_t address) {
	_address = address;
	Wire.begin(SDA,SCL);
	Wire.beginTransmission(_address);
	return (Wire.endTransmission() == 0);
}

uint8_t AT24C32::read(uint16_t index) {
	Wire.beginTransmission(_address);
	Wire.write((index >> 8) & 0x0F);
	Wire.write(index & 0xFF);
	if (Wire.endTransmission() == 0) {
		Wire.requestFrom(_address, (uint8_t)1);
		return Wire.read();
	}
	return 0;
}

void AT24C32::read(uint16_t index, uint8_t* buf, uint16_t len) {
	Wire.beginTransmission(_address);
	Wire.write((index >> 8) & 0x0F);
	Wire.write(index & 0xFF);
	if (Wire.endTransmission() == 0) {
		while (len > 0) {
			uint8_t l;

			l = EEPROM_WORK_SIZE;
			if (l > len)
				l = len;
			len -= l;
			Wire.requestFrom(_address, l);
			while (l--)
				*buf++ = Wire.read();
		}
	}
}

void AT24C32::write(uint16_t index, uint8_t data) {
	Wire.beginTransmission(_address);
	Wire.write((index >> 8) & 0x0F);
	Wire.write(index & 0xFF);
	Wire.write(data);
	Wire.endTransmission();
	delay(EEPROM_WRITE_TIMEOUT);
}

void AT24C32::write(uint16_t index, const uint8_t* buf, uint16_t len) {
	index &= 0x0FFF;
	while (len > 0) {
		uint8_t l;

		l = EEPROM_WORK_SIZE - (index % EEPROM_WORK_SIZE);
		if (l > len)
			l = len;
		len -= l;
		Wire.beginTransmission(_address);
		Wire.write(index >> 8);
		Wire.write(index & 0xFF);
		while (l--) {
			Wire.write(*buf++);
			++index;
		}
		if (Wire.endTransmission() != 0)
			break;
		while (!Wire.requestFrom(_address, (uint8_t)1)); // Polling EEPROM ready (write complete)
	}
}

