#include "Log.h"


String Logg::getAll(String divider) {
	String result = "";
	if (_log.length() > 0) {
		for (uint16_t i = 0; i < _log.length(); i++) {
			result += _log[i];
			if (i != _log.length() - 1) {
				result += divider;
			}
		}
	}
	else {
		result = "Log is empty...";
	}
	return result;
}

String Logg::getAll2Web(String divider) {
	String result = "";
	if (_log.length() > 0) {
		for (uint16_t i = 0; i < _log.length(); i++) {
			result += "<"+divider+">";
			result += _log[i];
			result += "</"+divider+">";
			
		}
	}
	else {
		result = "Log is empty...";
	}
	return result;
}


int Logg::length()
{
	return _log.length();
}

void Logg::clear(){
	_log.clear();
	}

void Logg::logging(String s)
{
#ifdef _SERIAL
	Serial.println(s);
#endif // _SERIAL

	if (_log.length() >= maxCount) {
		_log.pop_front();
	}
	_log.push_back(s);
}

Logg logg(100);