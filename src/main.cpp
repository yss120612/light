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
 
bool is_on;
HttpHelper * http_server;
unsigned long ms;
IRreceiver  ir(IRPIN);

void setup() {
  // put your setup code here, to run once:
   pinMode(LED,OUTPUT);
  

   pinMode(RELAY1,OUTPUT_OPEN_DRAIN);
   pinMode(RELAY2,OUTPUT_OPEN_DRAIN);
   pinMode(RELAY3,OUTPUT_OPEN_DRAIN);
   pinMode(RELAY4,OUTPUT_OPEN_DRAIN);
   digitalWrite(RELAY1,HIGH);
   digitalWrite(RELAY2,HIGH);
   digitalWrite(RELAY3,HIGH);
   digitalWrite(RELAY4,HIGH);
   data.relay1=false;
   data.relay2=false;
   data.relay3=false;
   
   is_on=false;
   //ir.enable();
   SPIFFS.begin();

   

// WiFi credentials.
   Serial.begin(9600);
    // Giving it a little time because the serial monitor doesn't
    // immediately attach. Want the firmware that's running to
    // appear on each upload.
    delay(2000);

    Serial.println();
    Serial.println("Running Firmware. v1.05");
    logg.logging("Running Firmware. v1.05");
    // Connect to Wifi.
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    logg.logging("Connecting to "+String(WIFI_SSID));
    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    //WiFi.disconnect();
   // delay(100);

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
      break;
      }
    }
    
    if (cycles<=4)
    {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    logg.logging("WiFi connected.IP address: "+WiFi.localIP().toString());
    Serial.println("Hello World, I'm connected to the internets!!");
    http_server = new HttpHelper();
    http_server->setup(&ir);
    }
    ms=0;
    ir.enable();
}
 

void loop() {

  
  if (millis()-ms<CHECKPERIOD) return;
  ms=millis();
  if (ir.checkIR(ms)>0)
  {
    //serialPrintUint64(dres.value,HEX);
    //Serial.println("");
    //Serial.print("Command:");
    //Serial.println(dres.command);
    //Serial.print("Address:");
    //Serial.println(dres.address);
    //irrecv.resume();

    
    //1
    if (ir.getCommand()==PULT_1){
      if (digitalRead(RELAY2)==HIGH){
        digitalWrite(RELAY2,LOW);
      }
      else{
        digitalWrite(RELAY2,HIGH);
      }
      data.relay1=digitalRead(RELAY2)==LOW;
    }
    //2
    if (ir.getCommand()==PULT_2){
      if (digitalRead(RELAY1)==HIGH){
        digitalWrite(RELAY1,LOW);
      }
      else{
        digitalWrite(RELAY1,HIGH);
      }
      data.relay2=digitalRead(RELAY1)==LOW;
    }
    //3
    if (ir.getCommand()==PULT_3){
      if (digitalRead(RELAY3)==HIGH){
        digitalWrite(RELAY3,LOW);
      }
      else{
        digitalWrite(RELAY3,HIGH);
      }
      data.relay3=digitalRead(RELAY3)==LOW;
    }
    //4
    if (ir.getCommand()==PULT_4){
      if (digitalRead(RELAY4)==HIGH){
        digitalWrite(RELAY4,LOW);
        delay(300);
        digitalWrite(RELAY4,HIGH);
       }
      
    }
    
    //POWER OFF
     if (ir.getCommand()==PULT_POWER){
        digitalWrite(RELAY1,HIGH);
        digitalWrite(RELAY2,HIGH);
        digitalWrite(RELAY3,HIGH);
        data.relayOff();
      //  digitalWrite(RELAY4,HIGH);
    }
    //SOUND OFF
     if (ir.getCommand()==PULT_SOUND){
        digitalWrite(RELAY1,HIGH);
        digitalWrite(RELAY2,HIGH);
        digitalWrite(RELAY3,HIGH);
        data.relayOff();
        digitalWrite(RELAY4,LOW);
        delay(300);
        digitalWrite(RELAY4,HIGH);
        
    }

  }
  if (!http_server || !http_server->isUpdate())
  {
    is_on=digitalRead(RELAY1)==LOW||digitalRead(RELAY2)==LOW||digitalRead(RELAY3)==LOW;
    digitalWrite(LED,!is_on?HIGH:LOW);  
  }
  //digitalWrite(LED,HIGH);
  //delay(100);
  //digitalWrite(LED,LOW);
}

