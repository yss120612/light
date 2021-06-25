#include <Arduino.h>

#include <SPIFFS.h>

#include "Log.h"
#include "Httphelper.h"
#include "Settings.h"
#include "Data.h"
#include "buttons.h"
#include "Blinker.h"



AppData data;

const char* WIFI_SSID = "Yss_GIGA";
const char* WIFI_PASS = "bqt3bqt3";
const char* fw = "Running firmware v. 2.2";



unsigned long ms;
unsigned long msWiFi;
boolean forceWiFi;//если не задалось с первого раза повторять каждые Х минут или нет
//BandLED band;
extern boolean connect2WiFi();


HttpHelper * http_server;
MqttClient *mqtt;

Blinker * blinker;



void init_networks(){
 if (!http_server)
        {
          http_server = new HttpHelper();
          http_server->setup(&data);
        }
        if (!mqtt)
        {
          mqtt = new MqttClient();
          mqtt->setup(&data);
          //logg.setup(mqtt);
        }
        
}

static void WiFiTaskHandler(void *pvParam) {
  const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.
}

void setup() {
  // put your setup code here, to run once:
   //pinMode(LED,OUTPUT);
  blinker=new Blinker(LED,HIGH);

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
   
   

// WiFi credentials.
#ifdef _SERIAL
   Serial.begin(115200);
#endif
    
    forceWiFi=true;
    
    if (connect2WiFi())
    {
     init_networks();
      msWiFi=0;
    }
    ms=0;
   data.setup(mqtt);  
   logg.logging(fw);
}



void loop()
{
  unsigned long m=millis();
  if (m<ms) ms=m;
  if (m - ms < CHECKPERIOD)
    return;
  ms = m;
  if (msWiFi==0) msWiFi=ms;
  

  data.loop(ms);
  if (mqtt) mqtt->loop(ms);

  // if (!http_server || !http_server->isUpdate())
  // {
  //   digitalWrite(LED, data.isOn() ? HIGH : LOW);
  // }


  if (blinker->getMode()!=BLINK_ON && data.isOn() || blinker->getMode()!=BLINK_OFF && !data.isOn()){
    blinker->setMode(data.isOn()?BLINK_ON:BLINK_OFF);
  }

  if (ms-msWiFi>CHECKWIFI){
    msWiFi=ms;
    if (forceWiFi && WiFi.status()!=WL_CONNECTED){
      connect2WiFi();
    }
  }
}

boolean connect2WiFi(){
    //Serial.print("Connecting to ");
    //Serial.println(WIFI_SSID);
    logg.logging("Connecting to "+String(WIFI_SSID));
    blinker->setMode(BLINK_4HZ);
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    //WiFi.disconnect();
   // delay(100);
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    //Serial.println("Connecting...");
    uint8_t cycles=0;
    while (WiFi.status() != WL_CONNECTED) {
      // Check to see if connecting failed.
      // This is due to incorrect credentials
      if (WiFi.status() == WL_CONNECT_FAILED) {
        logg.logging("Failed to connect to WIFI.");
        //Serial.println("Failed to connect to WIFI. Please verify credentials: ");
        //Serial.print("SSID: ");
        // Serial.println(WIFI_SSID);
        // Serial.print("Password: ");
        //Serial.println(WIFI_PASS);
        logg.logging("Connecting to "+String(WIFI_SSID));
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
    //Serial.println("");
    //Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
    blinker->setMode(BLINK_OFF);
    logg.logging("WiFi connected.IP address: "+WiFi.localIP().toString());
    //Serial.println("I'm connected to the internets!!");
    return true;
    
}