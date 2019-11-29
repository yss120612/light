#include <Arduino.h>

#include <SPIFFS.h>

#include "Log.h"
#include "IRreceiver.h"
#include "Httphelper.h"
#include "Settings.h"
#include "Data.h"

AppData data;

const char* WIFI_SSID = "Yss_GIGA";
const char* WIFI_PASS = "bqt3bqt3";
const char* fw = "Running firmware v. 2.0";

HttpHelper * http_server;
unsigned long ms;
unsigned long msWiFi;
boolean forceWiFi;//если не задалось с первого раза повторять каждые Х минут или нет
IRreceiver  ir(IRPIN);

extern boolean connect2WiFi();

void setup() {
  // put your setup code here, to run once:
   pinMode(LED,OUTPUT);
  

  //  pinMode(RELAY1,OUTPUT_OPEN_DRAIN);
  //  pinMode(RELAY2,OUTPUT_OPEN_DRAIN);
  //  pinMode(RELAY3,OUTPUT_OPEN_DRAIN);
  //  pinMode(RELAY4,OUTPUT_OPEN_DRAIN);
  //  digitalWrite(RELAY1,HIGH);
  //  digitalWrite(RELAY2,HIGH);
  //  digitalWrite(RELAY3,HIGH);
  //  digitalWrite(RELAY4,HIGH);
   //data.relay1=false;
   //data.relay2=false;
   //data.relay3=false;
   

   SPIFFS.begin();
   data.setup(); 
   

// WiFi credentials.
   Serial.begin(9600);
    // Giving it a little time because the serial monitor doesn't
    // immediately attach. Want the firmware that's running to
    // appear on each upload.
    delay(2000);

    Serial.println();
    Serial.print(fw);
    logg.logging(fw);
    // Connect to Wifi.
    
    forceWiFi=true;
    
    if (connect2WiFi())
    {
      http_server = new HttpHelper();
      http_server->setup(&ir,&data);
      msWiFi=0;
    }
    ms=0;
    ir.enable();
}



void loop()
{

  if (millis() - ms < CHECKPERIOD)
    return;
  ms = millis();
  if (msWiFi==0) msWiFi=ms;
  if (ir.checkIR(ms) > 0)
  {
    switch (ir.getCommand())
    {
    case PULT_1:
      data.relaySwitch(0, ms);
      break;
    case PULT_2:
      data.relaySwitch(1, ms);
      break;
    case PULT_3:
      data.relaySwitch(2, ms);
      break;
    case PULT_4:
      data.relaySwitch(3, ms);
      break;
    case PULT_POWER:
      data.relayOff(ms);
      break;
    case PULT_SOUND:
      data.relaySwitchOff(ms);
      break;
    }
  }

  data.loop(ms);
  
  if (!http_server || !http_server->isUpdate())
  {
    digitalWrite(LED, data.isOn() ? LOW : HIGH);
  }

  if (ms-msWiFi>CHECKWIFI){
    msWiFi=ms;
    if (forceWiFi && WiFi.status()!=WL_CONNECTED){
      connect2WiFi();
    }
  }
}

boolean connect2WiFi(){
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    logg.logging("Connecting to "+String(WIFI_SSID));
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    //WiFi.disconnect();
   // delay(100);
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.println("Connecting...");
    uint8_t cycles=0;
    while (WiFi.status() != WL_CONNECTED) {
      // Check to see if connecting failed.
      // This is due to incorrect credentials
      if (WiFi.status() == WL_CONNECT_FAILED) {
        Serial.println("Failed to connect to WIFI. Please verify credentials: ");
        Serial.print("SSID: ");
        Serial.println(WIFI_SSID);
        Serial.print("Password: ");
        Serial.println(WIFI_PASS);

      }
      delay(5000);
      cycles++;
      if (cycles>4) 
      {
      Serial.println("Working witout WiFi :(");  
      return false;
      break;
      }
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    logg.logging("WiFi connected.IP address: "+WiFi.localIP().toString());
    Serial.println("I'm connected to the internets!!");
    return true;
}