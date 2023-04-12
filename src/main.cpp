#include <Arduino.h>


#include <SPIFFS.h>

#include "Log.h"
//#include "Httphelper.h"
#include "Settings.h"
#include "Data.h"
//#include "buttons.h"
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
#include "BTNTask.h"



AppData data;

//const char* WIFI_SSID = "Yss_GIGA";
//const char* WIFI_PASS = "bqt3bqt3";
const char* fw = "Running firmware v. 3.0";



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
MessageBufferHandle_t display_message,alarm_messages,web_messages;



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
BTNTask * btn;
//extern void init_networks();



// static void WiFiTaskHandler(void *pvParam) {
//   const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.
// }

void setup() {
 
  //blinker=new Blinker(LED,HIGH);
queue= xQueueCreate(16,sizeof(event_t));
btn_semaphore=xSemaphoreCreateBinary();
flags=xEventGroupCreate();
display_message=xMessageBufferCreate(DISP_MESSAGE_LENGTH+4);
alarm_messages=xMessageBufferCreate(SSTATE_LENGTH+4);//=length label
web_messages=xMessageBufferCreate(SSTATE_LENGTH+4);
#ifdef _SERIAL
   Serial.begin(115200);
#endif

//SPIFFS.begin();
mem= new MEMTask("Memory",2048,queue,alarm_messages,web_messages);  
mem->resume();
leds = new LEDTask("Leds",3072,queue,HIGH);
leds->resume();
wifi=new WiFiTask("WiFi",4096,queue,flags);
wifi->resume();
ir= new IRTask("IR",2048,queue);  
ir->resume();
http = new HTTPTask("http",4096,queue,flags,web_messages);
http->resume();
rtc = new RTCTask("Clock",2048,flags,queue, display_message,alarm_messages);  
rtc->resume();
display= new DISPTask("Display",2048, display_message);  
display->resume();
relay= new RELTask("Relay",3072,queue,display_message);  
relay->resume();
band= new BANDTask("Band",2048, queue,display_message, HIGH);  
band->resume();
btn= new BTNTask("Buttons",2048, queue);  
btn->resume();
}

void web_event(event_t event){
  notify_t notify;
  switch (event.button){
    case 1://set alarm from web
     notify.title=1;
     notify.alarm=event.alarm;
     rtc->notify(notify);
  break;
  case 2://print all alarms
     notify.title=11;
     notify.packet.var=0;
     notify.packet.value=0;
     rtc->notify(notify);
  break;
  case 3://print active alarm
     notify.title=12;
     notify.packet.var=0;
     notify.packet.value=0;
     rtc->notify(notify);
  break;
  case 4://reset all alarms
     notify.title=13;
     notify.packet.var=0;
     notify.packet.value=0;
     rtc->notify(notify);
  break;
  case 11:
  case 12:
  case 13:
  case 14://set relays from web
     notify.title=event.button-10;
     notify.packet.var=0;//save in memory
     notify.packet.value=event.count;
     rtc->notify(notify);
  break;

      case WEB_CANNEL_CW:
      case WEB_CANNEL_NW:
      case WEB_CANNEL_WW:
        notify.title=1;
        notify.packet.var=event.button-100;
        notify.packet.value=event.count;
        rtc->notify(notify);
        break;
  }
}


void mem_event(event_t event){
  notify_t notify;
  
  switch (event.button){
        //read result processing
        case 1://CW
        case 2://NW
        case 3://WW
         notify.title=10;
         notify.packet.var=event.button-1;
         notify.packet.value=event.count;
         band->notify(notify);
        break;
        
        case 4://relay 1
        case 5://relay 2
        case 6://relay 3
        case 7://relay 4
          notify.title=event.button-3;
         notify.packet.var=1;
         notify.packet.value=event.count>0?1:0;
         relay->notify(notify);
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
          notify.title=1;
          notify.packet.var=0;
          notify.packet.value=event.button-100;
          mem->notify(notify);
        break;
        ///// request for write
        case 200://
        case 201:
        case 202:
             notify.title=event.button-200+20;
             notify.packet.var=(uint8_t)BLINK_ON;
             notify.packet.value=event.count;
             mem->notify(notify);
        break;
        case 203:
        case 204:
        case 205:
        case 206:
          notify.title=event.button-203+10;
          notify.packet.value=event.count;
        break;
      
  }
}


void buton_event(event_t nt){
  notify_t notify;
  uint32_t command;
  switch (nt.state){
  case BTN_CLICK:
  switch (nt.button){
  case 0:
      if (nt.count==1){
        notify.title=10;
        rtc->notify(notify);
      }
  break;
  case 1:
      
  break;
  }
  break;
  case BTN_LONGCLICK:
  switch (nt.button){
  case 0:
      Serial.print("Long click after ");
      Serial.println(nt.count);
      if (nt.count=3){
        ESP.restart();
      }
  break;
  case 1:
  break;
  }
  }
}
void display_event(event_t command)
{
notify_t nt;  
switch (command.button)
      {
    case 11:
    case 12:
    case 13:
    case 14:
    //uint32_t result=makePacket(command.button-10,0,command.count);
    //nt.title=command.button-10;
    //display->notify(nt);
    break;
    case 20://show time
    break;
      }
}

void pult_event(event_t command)
{

bool learn_command=true;
uint32_t result;

if (learn_command){
        //display.showString("Dev."+String(command.count)+(command.count==IR_DEVICE?" MY":" ALIEN"),"Code "+String(command.button),"type="+String(command.type));
      }
      if (command.count!=IR_DEVICE) return;
      switch (command.button)
      {
      case PULT_1:
         result=makePacket(11,0,0);
         relay->notify(result);
        break;
      case PULT_2:
         result=makePacket(12,0,0);
         relay->notify(result);
        break;
      case PULT_3:
         result=makePacket(13,0,0);
         relay->notify(result);
        break;
      case PULT_4:
         result=makePacket(14,0,0);
         relay->notify(result);
        break;
      
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
          result=makePacket(10,0,0);//request time;
          rtc->notify(result);
        break;
      case PULT_FASTFORWARD:
      
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
}

void led_event(event_t command){
uint32_t result;
switch (command.button)
    {
    case 111:
    result=makePacket(command.button,0,0);
    leds->notify(result);
    break;
    case 113:
    result=makePacket(command.button,0,0);
    leds->notify(result);
    
    
    break;
      }
}

void loop()
{

event_t command;
if (xQueueReceive(queue,&command,portMAX_DELAY))
{
  

  switch(command.state)
  {
  case WEB_EVENT:
      web_event(command);
  break;
  case BTN_CLICK:
  case BTN_LONGCLICK:
      buton_event(command);
  break;
  case MEM_EVENT:
      mem_event(command);
  break;
  case DISP_EVENT:
      display_event(command);
  break;
  case LED_EVENT:
      led_event(command);
  break;
  case PULT_BUTTON:
      pult_event(command);
  break;
 }
}//if queue

}