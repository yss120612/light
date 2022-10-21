#include <Arduino.h>

#include <SPIFFS.h>

#include "Log.h"
//#include "Httphelper.h"
#include "Settings.h"
#include "Data.h"
#include "buttons.h"
//#include "Blinker.h"
#include "MEMTask.h"
#include "WiFiTask.h"
#include "IRTask.h"
#include "LEDTask.h"
#include "HTTPTask.h"
#include "RELTask.h"
#include "BANDTask.h"



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
BANDTask * band;
MEMTask * mem;
//extern void init_networks();



// static void WiFiTaskHandler(void *pvParam) {
//   const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.
// }

void setup() {
 
  //blinker=new Blinker(LED,HIGH);
queue= xQueueCreate(16,sizeof(event_t));
btn_semaphore=xSemaphoreCreateBinary();
flags=xEventGroupCreate();
#ifdef _SERIAL
   Serial.begin(115200);
#endif

//SPIFFS.begin();
mem= new MEMTask("Memory",2048,queue);  
mem->resume();
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
band= new BANDTask("Band",2048, queue, HIGH);  
band->resume();

// WiFi credentials.

    
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
      
        //result=1<<16 | command.count > 0;
        result=makePacket(1,0,command.count > 0?1:0);
        relay->notify(result);
        break;
      case PULT_2:
        result=makePacket(2,0,command.count > 0?1:0);
        //result=2<<16 & 0xFFFF0000 | command.count > 0;
        relay->notify(result);
        break;
      case PULT_3:
        result=makePacket(3,0,command.count > 0?1:0);
        //result=3<<16 & 0xFFFF0000 | command.count > 0;
        relay->notify(result);
        break;
      case PULT_4:
        result=makePacket(4,0,command.count > 0?1:0);
        //result=4<<16 & 0xFFFF0000 | command.count > 0;
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
  case BTN_CLICK:
  break;
  case MEM_EVENT:
      switch (command.button)
      {
        //read result processing
        case 1://CW
        result=makePacket(10,0,command.count);
        //1<<24 & 0xFF000000 | 0 << 16 & 0x00FF0000 | command.count & 0x0000FFFF;
        band->notify(result);
        break;
        case 2://NW
        result=makePacket(10,1,command.count);
        //1<<24 & 0xFF000000 | 1 << 16 & 0x00FF0000 | command.count & 0x0000FFFF;
        band->notify(result);
        break;
        case 3://WW
        result=makePacket(10,2,command.count);
        //1<<24 & 0xFF000000 | 2 << 16 & 0x00FF0000 | command.count & 0x0000FFFF;
        band->notify(result);
        break;
        case 4://relay 1
          result=makePacket(1,1,command.count>0?1:0);
          //1<<16 & 0xFFFF0000 | command.count > 0?1:0 & 0x0000FFFF;
          relay->notify(result);
        break;
        case 5://relay 2
            result=makePacket(2,1,command.count>0?1:0);
          //result=2<<16 & 0xFFFF0000 | command.count > 0?1:0 & 0x0000FFFF ;
          relay->notify(result);
        break;
        case 6://relay 3
          result=makePacket(3,1,command.count>0?1:0);
          //result=3<<16 & 0xFFFF0000 | command.count > 0?1:0 & 0x0000FFFF;
          relay->notify(result);
        break;
        case 7://relay 4
          result=makePacket(4,1,command.count>0?1:0);
          //result=4<<16 & 0xFFFF0000 | command.count > 0?1:0 & 0x0000FFFF;
          relay->notify(result);
        break;

        ///// request for read
        case 100://
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 106:
        case 107:
          result=makePacket(1,0,command.button-100);
          mem->notify(result);
        break;
        // case 101://request CW
        //   result=makePacket(1,0,1);
        //   //result=1<<24 & 0xFF000000 | 1 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 102://request NW
        //   result=makePacket(1,0,2);
        //   //result=1<<24 & 0xFF000000 | 2 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 103://request WW
        //   result=makePacket(1,0,3);
        //   //result=1<<24 & 0xFF000000 | 3 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 104://request Relay1
        //   result=makePacket(1,0,0);
        //   //result=1<<24 & 0xFF000000 | 4 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 105://request Relay2
        //   result=1<<24 & 0xFF000000 | 5 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 106://request Relay3
        //   result=1<<24 & 0xFF000000 | 6 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 107://request Relay4
        //   result=1<<24 & 0xFF000000 | 7 & 0x0000FFFF;
        //   mem->notify(result);
        // break;

         ///// request for write
        case 200://
        case 201:
        case 202:
        case 203:
        case 204:
        case 205:
        case 206:
        case 207:
          result=makePacket(2,command.count,command.button-200);
          //result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 0 & 0x0000FFFF;
          mem->notify(result);
        break;
        // case 201://request CW
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 1 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 202://request NW
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 2 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 203://request WW
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 3 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 204://request Relay1
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 4 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 205://request Relay2
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 5 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 206://request Relay3
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 6 & 0x0000FFFF;
        //   mem->notify(result);
        // break;
        // case 207://request Relay4
        //   result=2<<24 & 0xFF000000 | command.count <16 & 0x000FF0000 | 7 & 0x0000FFFF;
        //   mem->notify(result);
        // break;


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
        result=makePacket(11,0,0);
        //result=11<<16 & 0xFFFF0000 | 0;
        relay->notify(result);
        
        break;
      case PULT_2:
        result=makePacket(12,0,0);
        //result=12<<16 & 0xFFFF0000 | 0;
        relay->notify(result);
        break;
      case PULT_3:
        result=makePacket(13,0,0);
        //result=13<<16 & 0xFFFF0000 | 0;
        relay->notify(result);
        break;
      case PULT_4:
        //result=14<<16 & 0xFFFF0000 | 0;
        result=makePacket(14,0,0);
        relay->notify(result);
        break;
      case PULT_INFO:
        conf.print();
        break;
      case PULT_POWER:
        result=makePacket(20,0,0);
        //result=20<<16 & 0xFFFF0000 | 0;
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
        result=makePacket(9,0,0);
        //result=9<<24 & 0xFF000000;
        band->notify(result);
        //setOneBand(CANNEL_CW, 0);
        //setOneBand(CANNEL_NW, 0);
        //setOneBand(CANNEL_WW, 64);
        //logg.logging("SLOW");
        break;
      case PULT_ZOOM: //low
        //result=5<<24 & 0xFF000000;
        result=makePacket(5,0,0);
        band->notify(result);
        //setOneBand(CANNEL_CW, 64);
        //setOneBand(CANNEL_NW, 64);
        //setOneBand(CANNEL_WW, 64);
        break;
      case PULT_STOP: //middle
        //result=4<<24 & 0xFF000000;
        result=makePacket(4,0,0);
        band->notify(result);
        //setOneBand(CANNEL_CW, 128);
        //setOneBand(CANNEL_NW, 128);
        //setOneBand(CANNEL_WW, 128);
        break;
      case PULT_PAUSE: //full
        //result=2<<24 & 0xFF000000;
        result=makePacket(2,0,0);
        band->notify(result);
        //setOneBand(CANNEL_CW, 255);
        //setOneBand(CANNEL_NW, 255);
        //setOneBand(CANNEL_WW, 255);
        break;
      }
      break;
    }
}//if queue

  // unsigned long m=millis();
  // if (m<ms) ms=m;
  // if (m - ms < CHECKPERIOD)
  //   return;
  // ms = m;
  //if (msWiFi==0 || msWiFi>ms) msWiFi=ms;
  

  //data.loop(ms);
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