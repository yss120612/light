#include "Config.h"
#include <SPIFFS.h>
#include "Log.h"
#include "ArduinoJson.h"

const char * filename = "/config.txt";

Config::Config(){

}






void Config::load() {
  // Open file for reading
  File file = SPIFFS.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<256> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  logg.logging(F("Failed to read file, using default configuration"));

  // Copy values from the JsonDocument to the Config
  cw = doc["CW"] | 128;
  nw = doc["NW"] | 128;
  ww = doc["WW"] | 128;
  //strlcpy(config.hostname,                  // <- destination
  //        doc["hostname"] | "example.com",  // <- source
  //        sizeof(config.hostname));         // <- destination's capacity

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
}

// Saves the configuration to a file
void Config::save() {
  // Delete existing file, otherwise the configuration is appended to the file
  SPIFFS.remove(filename);

  // Open file for writing
  File file = SPIFFS.open(filename, FILE_WRITE);
  if (!file) {
    logg.logging(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<256> doc;

  // Set the values in the document
  doc["CW"] = cw;
  doc["NW"] = nw;
  doc["WW"] = ww;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    logg.logging(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

// Prints the content of a file to the Serial
void Config::print() {
  // Open file for reading
  File file = SPIFFS.open(filename);
  if (!file) {
    logg.logging(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    logg.logging(String(file.read()));
  }
 // Serial.println();

  // Close the file
  file.close();
}


Config conf;