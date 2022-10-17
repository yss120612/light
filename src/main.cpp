#include <Arduino.h>

#include <SPIFFS.h>

#include "Log.h"
//#include "Httphelper.h"
#include "Settings.h"
#include "Data.h"
#include "buttons.h"
//#include "Blinker.h"
#include "WiFiTask.h"
#include "IRTask.h"
#include "LEDTask.h"
#include "HTTPTask.h"
#include "RELTask.h"



AppData data;

//const char* WIFI_SSID = "Yss_GIGA";
//const char* WIFI_PASS = "bqt3bqt3";
const char* fw = "Running firmware v. 2.2";



unsigned long ms;
//unsigned long msWiFi;
//boolean forceWiFi;//если не задалось с первого раза повторять каждые Х минут или нет
//BandLED band;
//extern boolean connect2WiFi();


//HttpHelper * http_server;
//MqttClient *mqtt;
QueueHandle_t queue;
SemaphoreHandle_t btn_semaphore;
EventGroupHandle_t flags;

//Blinker * blinker;
LEDTask * leds;
WiFiTask * wifi;
IRTask * ir;
HTTPTask * http;
RELTask * relay;
//extern void init_networks();



// static void WiFiTaskHandler(void *pvParam) {
//   const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.
// }

void setup() {
 
  //blinker=new Blinker(LED,HIGH);
queue= xQueueCreate(16,sizeof(event_t));
btn_semaphore=xSemaphoreCreateBinary();
flags=xEventGroupCreate();


//SPIFFS.begin();
leds = new LEDTask("Leds",3072,queue,LOW);
leds->resume();
wifi=new WiFiTask("WiFi",8192,queue,flags);
wifi->resume();
ir= new IRTask("IR",2048,queue);  
ir->resume();
http = new HTTPTask("http",8192,queue,flags);
http->resume();
relay= new RELTask("Relay",2048,queue);  
relay->resume();

// WiFi credentials.
#ifdef _SERIAL
   Serial.begin(9600);
#endif
    
    // forceWiFi=true;
    
    // if (connect2WiFi())
    // {
    //  init_networks();
    //   msWiFi=0;
    // }
    ms=0;
   //data.setup(mqtt);  
   logg.logging(fw);
}



void loop()
{

event_t command;
if (xQueueReceive(queue,&command,portMAX_DELAY))
{
  bool learn_command=true;
  uint32_t result;
  switch(command.state)
  {
  case WEB_EVENT:
      switch (command.button)
      {
      case PULT_1:
        result=1<<16 | command.count > 0;
        relay->notify(result);
        break;
      case PULT_2:
        result=2<<16 | command.count > 0;
        relay->notify(result);
        break;
      case PULT_3:
        result=3<<16 | command.count > 0;
        relay->notify(result);
        break;
      case PULT_4:
        result=4<<16 | command.count > 0;
        relay->notify(result);
        break;
      case WEB_CANNEL_CW:
        //setOneBand(CANNEL_CW, ev.count);
        break;
      case WEB_CANNEL_NW:
        //setOneBand(CANNEL_NW, ev.count);
        break;
      case WEB_CANNEL_WW:
        //setOneBand(CANNEL_WW, ev.count);
        break;
      }
      break;
       case PULT_BUTTON:
      if (learn_command){
        //display.showString("Dev."+String(command.count)+(command.count==IR_DEVICE?" MY":" ALIEN"),"Code "+String(command.button),"type="+String(command.type));
      }
      if (command.count!=IR_DEVICE) break;
      switch (command.button)
      {
      case PULT_1:
        result=11<<16 | 0;
        relay->notify(result);
        break;
      case PULT_2:
        result=12<<16 | 0;
        relay->notify(result);
        break;
      case PULT_3:
        result=13<<16 | 0;
        relay->notify(result);
        break;
      case PULT_4:
        result=14<<16 | 0;
        relay->notify(result);
        break;
      
      
      case PULT_INFO:
        conf.print();
        break;
      case PULT_POWER:
        result=20<<16 | 0;
        relay->notify(result);
        break;
      case PULT_SOUND:
        //relaySwitchOff(t);
        break;
      case PULT_VOLDOWN:
        //tuneLight(false, CANNEL_CW);
        break;
      case PULT_VOLUP:
        //tuneLight(true, CANNEL_CW);
        break;
      case PULT_FASTBACK:
        //tuneLight(false, CANNEL_NW);
        break;
      case PULT_FASTFORWARD:
        //tuneLight(true, CANNEL_NW);
        break;
      case PULT_PREV:
        //tuneLight(false, CANNEL_WW);
        break;
      case PULT_NEXT:
        //tuneLight(true, CANNEL_WW);
        break;
      case PULT_SLOW: //ultra low
        //setOneBand(CANNEL_CW, 0);
        //setOneBand(CANNEL_NW, 0);
        //setOneBand(CANNEL_WW, 64);
        //logg.logging("SLOW");
        break;
      case PULT_ZOOM: //low
        //setOneBand(CANNEL_CW, 64);
        //setOneBand(CANNEL_NW, 64);
        //setOneBand(CANNEL_WW, 64);
        break;
      case PULT_STOP: //middle
        //setOneBand(CANNEL_CW, 128);
        //setOneBand(CANNEL_NW, 128);
        //setOneBand(CANNEL_WW, 128);
        break;
      case PULT_PAUSE: //full
        //setOneBand(CANNEL_CW, 255);
        //setOneBand(CANNEL_NW, 255);
        //setOneBand(CANNEL_WW, 255);
        break;
      }
      break;
    }
}//if queue

  unsigned long m=millis();
  if (m<ms) ms=m;
  if (m - ms < CHECKPERIOD)
    return;
  ms = m;
  //if (msWiFi==0 || msWiFi>ms) msWiFi=ms;
  

  data.loop(ms);
  //if (mqtt) mqtt->loop(ms);

  //if (!http_server || !http_server->isUpdate())
  //{
    //digitalWrite(LED, data.isOn() ? HIGH : LOW);
  //}

  // if (blinker)
  // if (blinker->getMode()!=BLINK_ON && data.isOn() || blinker->getMode()!=BLINK_OFF && !data.isOn()){
  //   blinker->setMode(data.isOn()?BLINK_ON:BLINK_OFF);
  // }

  // if (ms-msWiFi>CHECKWIFI){
  //   msWiFi=ms;
  //   if (forceWiFi && WiFi.status()!=WL_CONNECTED){
  //     connect2WiFi();
  //   }
  // }
}

void init_networks(){
//  if (!http_server)
//         {
//           http_server = new HttpHelper();
//           http_server->setup(&data);
//         }
//         if (!mqtt)
//         {
//           mqtt = new MqttClient();
//           mqtt->setup(&data);
//         }
        
}

// boolean connect2WiFi(){
//     //Serial.print("Connecting to ");
//     //Serial.println(WIFI_SSID);
//     //return false;
//     logg.logging("Connecting to "+String(WIFI_SSID));
//     //////////////////////////////////data.getI2Cdevices();
//     blinker->setMode(BLINK_4HZ);
//     // Set WiFi to station mode and disconnect from an AP if it was previously connected
//     WiFi.mode(WIFI_STA);
//     //WiFi.disconnect();
//    // delay(100);
//     //data.getI2Cdevices();
//     WiFi.disconnect();
//     WiFi.begin(WIFI_SSID, WIFI_PASS);
//     //Serial.println("Connecting...");
//     uint8_t cycles=0;
//     while (WiFi.status() != WL_CONNECTED) {
//       // Check to see if connecting failed.
//       // This is due to incorrect credentials
//       if (WiFi.status() == WL_CONNECT_FAILED) {
//         logg.logging("Failed to connect to WIFI.");
//         //Serial.println("Failed to connect to WIFI. Please verify credentials: ");
//         //Serial.print("SSID: ");
//         // Serial.println(WIFI_SSID);
//         // Serial.print("Password: ");
//         //Serial.println(WIFI_PASS);
//         logg.logging("Connecting to "+String(WIFI_SSID));
//       }
//       delay(7000);
//       cycles++;
//       if (cycles>4) 
//       {
//       Serial.println("Working witout WiFi :(");  
//       return false;
//       break;
//       }
//     }
//     //Serial.println("");
//     //Serial.println("WiFi connected");
//     //Serial.println("IP address: ");
//     //Serial.println(WiFi.localIP());
//     blinker->setMode(BLINK_OFF);
//     logg.logging("WiFi connected.IP address: "+WiFi.localIP().toString());
//     //Serial.println("I'm connected to the internets!!");
//     return true;
    
// }