#ifndef _LOG_h
#define _LOG_h
#define _SERIAL1

#include <Arduino.h>

#include<QList.h>

class Logg
{
public:
	Logg(uint16_t max) { maxCount = max; };
	~Logg() { _log.clear(); }
	void logging(String s);
	String getAll(String divider = "");
	String getAll2Web(String divider = "li");
	int length();
	void clear();
private:
	uint16_t maxCount;
	QList<String> _log;
};


extern Logg logg;


#endif