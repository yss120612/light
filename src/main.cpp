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
#include "DISPTask.h"
#include "RTCTask.h"



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
MEMTask * mem;
LEDTask * leds;
WiFiTask * wifi;
IRTask * ir;
HTTPTask * http;
DISPTask * display;
RTCTask * rtc;
RELTask * relay;
BANDTask * band;
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
wifi=new WiFiTask("WiFi",4096,queue,flags);
wifi->resume();
ir= new IRTask("IR",2048,queue);  
ir->resume();
http = new HTTPTask("http",4096,queue,flags);
http->resume();
rtc = new RTCTask("Clock",2048,queue,flags);  
rtc->resume();
display= new DISPTask("Display",2048);  
display->resume();
relay= new RELTask("Relay",3072,queue);  
relay->resume();
band= new BANDTask("Band",2048, queue, HIGH);  
band->resume();
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
        result=makePacket(1,0,command.count > 0?1:0);
        relay->notify(result);
        display->notify(result);
        break;
      case PULT_2:
        result=makePacket(2,0,command.count > 0?1:0);
        relay->notify(result);
        display->notify(result);
        break;
      case PULT_3:
        result=makePacket(3,0,command.count > 0?1:0);
        relay->notify(result);
        display->notify(result);
        break;
      case PULT_4:
        result=makePacket(4,0,command.count > 0?1:0);
        relay->notify(result);
        display->notify(result);
        break;
      case WEB_CANNEL_CW:
        //setOneBand(CANNEL_CW, ev.count);
        result=makePacket(1,CANNEL_CW,command.count);
        band->notify(result);
        break;
      case WEB_CANNEL_NW:
        result=makePacket(1,CANNEL_NW,command.count);
        band->notify(result);
        break;
      case WEB_CANNEL_WW:
        result=makePacket(1,CANNEL_WW,command.count);
        band->notify(result);
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
        case 2://NW
        case 3://WW
         Serial.print("restore band ");
         Serial.print(command.button);
         Serial.print(" state=");
         Serial.println(command.count);
        result=makePacket(10,command.button-1,command.count);
        band->notify(result);
        break;
        
        case 4://relay 1
        case 5://relay 2
        case 6://relay 3
        case 7://relay 4
          delay(100);
          result=makePacket(command.button-3,1,command.count>0?1:0);
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
          delay(800);
        break;
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
             mem->notify(result);
             delay(800);
        break;
      }
  
  break;
  case DISP_EVENT:
  switch (command.button)
      {
    case 11:
    case 12:
    case 13:
    case 14:
    result=makePacket(command.button-10,0,command.count);
    display->notify(result);
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
        result=makePacket(11,0,0);
         relay->notify(result);
         //display->notify(result);
        break;
      case PULT_2:
        result=makePacket(12,0,0);
        //display->notify(result);
        relay->notify(result);
        break;
      case PULT_3:
        result=makePacket(13,0,0);
        //display->notify(result);
        relay->notify(result);
        break;
      case PULT_4:
        result=makePacket(14,0,0);
        //display->notify(result);
        relay->notify(result);
        break;
      // case PULT_INFO:
      //   conf.print();
      //   break;
      case PULT_POWER:
        result=makePacket(20,0,0);
        relay->notify(result);
        break;
      case PULT_SOUND:
        break;
      case PULT_VOLDOWN:
        break;
      case PULT_VOLUP:
        break;
      case PULT_FASTBACK:
        //tuneLight(false, CANNEL_NW);
        break;
      case PULT_FASTFORWARD:
        //tuneLight(true, CANNEL_NW);
        break;
 
      case PULT_PREV: //ultra low
        result=makePacket(9,0,0);
        band->notify(result);
        break;
      case PULT_PAUSE: //low
        result=makePacket(5,0,0);
        band->notify(result);
        break;
      case PULT_STOP: //middle
        result=makePacket(4,0,0);
        band->notify(result);
        break;
      case PULT_NEXT: //full
        result=makePacket(2,0,0);
        band->notify(result);
        break;
      }
      break;
    }
}//if queue

}