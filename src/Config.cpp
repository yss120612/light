#include "Config.h"
#include <SPIFFS.h>
#include "Log.h"
#include "ArduinoJson.h"

//const char * filename = "/config.txt";

Config::Config(){

}

bool Config::setup(){
 return at24.begin();
}

uint16_t Config::calcLength()
{
	//uint8_t length = sizeof(uint8_t)*3+sizeof(boolean)*3;
 	return sizeof(uint8_t)*3+sizeof(boolean)*3;
}

void Config::upload(){
uint16_t	length = calcLength();
uint16_t  addr=0x78;
uint8_t * buff = new uint8_t[length];
at24.read(addr, buff, length);
uint16_t idx = 0;
bool tmp;
memcpy(&cw,(buff + idx),  sizeof(cw)); idx += sizeof(cw);
memcpy(&nw, (buff + idx),  sizeof(nw)); idx += sizeof(nw);
memcpy(&ww, (buff + idx),  sizeof(ww)); idx += sizeof(ww);
memcpy(&tmp, (buff + idx),  sizeof(tmp)); idx += sizeof(tmp);notebook_on=(bool)tmp;
memcpy(&tmp, (buff + idx),  sizeof(tmp)); idx += sizeof(tmp);lamp_on=(bool)tmp;
memcpy(&tmp, (buff + idx),  sizeof(tmp)); idx += sizeof(tmp);heater_on=(bool)tmp;
delete buff;
chg=false;
}

void Config::story(){
if (!chg) return;

	uint16_t	length = calcLength();
	uint8_t * buff = new uint8_t [length];
  uint16_t  addr=0x78;
	uint16_t idx = 0;
  bool temp;


	memcpy((buff + idx), &cw,		sizeof(cw)); idx += sizeof(cw);
	memcpy((buff + idx), &nw,	sizeof(nw)); idx += sizeof(nw);
	memcpy((buff + idx), &ww,	sizeof(ww)); idx += sizeof(ww);
  temp=notebook_on;
  memcpy((buff + idx), &temp,	sizeof(temp)); idx += sizeof(temp);
  temp=lamp_on;
  memcpy((buff + idx), &temp,	sizeof(temp)); idx += sizeof(temp);
  temp=heater_on;
  memcpy((buff + idx), &temp,	sizeof(temp)); idx += sizeof(temp);

  //memcpy((buff + idx), &lamp_on,	sizeof(lamp_on)); idx += sizeof(lamp_on);
  //memcpy((buff + idx), &heater_on,	sizeof(heater_on)); idx += sizeof(heater_on);
  

  at24.write(addr, buff, length);
	logg.logging("CONFIG saved ("+String(idx)+" bytes)!");
	chg = false;
	delete buff;
}

void Config::force_story(){
chg=true;
story();
}


// void Config::load() {
//   // Open file for reading
//   File file = SPIFFS.open(filename);

//   // Allocate a temporary JsonDocument
//   // Don't forget to change the capacity to match your requirements.
//   // Use arduinojson.org/v6/assistant to compute the capacity.
//   StaticJsonDocument<256> doc;

//   // Deserialize the JSON document
//   DeserializationError error = deserializeJson(doc, file);
//   if (error)
//   logg.logging(F("Failed to read file, using default configuration"));

//   // Copy values from the JsonDocument to the Config
//   cw = doc["CW"] | 128;
//   nw = doc["NW"] | 128;
//   ww = doc["WW"] | 128;
//   //strlcpy(config.hostname,                  // <- destination
//   //        doc["hostname"] | "example.com",  // <- source
//   //        sizeof(config.hostname));         // <- destination's capacity

//   // Close the file (Curiously, File's destructor doesn't close the file)
//   file.close();
// }

// // Saves the configuration to a file
// void Config::save() {
//   // Delete existing file, otherwise the configuration is appended to the file
//   SPIFFS.remove(filename);

//   // Open file for writing
//   File file = SPIFFS.open(filename, FILE_WRITE);
//   if (!file) {
//     logg.logging(F("Failed to create file"));
//     return;
//   }

//   // Allocate a temporary JsonDocument
//   // Don't forget to change the capacity to match your requirements.
//   // Use arduinojson.org/assistant to compute the capacity.
//   StaticJsonDocument<256> doc;

//   // Set the values in the document
//   doc["CW"] = cw;
//   doc["NW"] = nw;
//   doc["WW"] = ww;

//   // Serialize JSON to file
//   if (serializeJson(doc, file) == 0) {
//     logg.logging(F("Failed to write to file"));
//   }

//   // Close the file
//   file.close();
// }

// Prints the content of a file to the Serial
void Config::print() {
  // Open file for reading
  String res="Lamp_ON="+String((bool)conf.lamp_on);
  res+="; Notebook_ON="+String(conf.notebook_on);
  res+="; Ion_ON="+String(conf.heater_on);
  res+="; CW="+String(conf.cw);
  res+="; NW="+String(conf.nw);
  res+="; WW="+String(conf.ww);
  logg.logging(res);
//   File file = SPIFFS.open(filename);
//   if (!file) {
//     logg.logging(F("Failed to read file"));
//     return;
//   }

//   // Extract each characters by one by one
//   while (file.available()) {
//     logg.logging(String(file.read()));
//   }
//  // Serial.println();

//   // Close the file
//   file.close();
}


Config conf;