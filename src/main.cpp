#include <Arduino.h>
#include "Settings.h"

#include <MEMTask.h>
#include <WiFiTask.h>
#include <IRTask.h>
#include <ENCTask.h>
#include <LEDTask.h>
#include <RELTask.h>
#include <DISPTask.h>
#include <RTCTask.h>

#include "HTTP2Task.h"


//AppData data;

//const char* WIFI_SSID = "Yss_GIGA";
//const char* WIFI_PASS = "bqt3bqt3";
//const char* fw = "Running firmware v. 3.0";



//unsigned long ms;
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
//BANDTask * band;
ENCTask * btn;
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
#ifdef DEBUGG
   Serial.begin(115200);
#endif

//SPIFFS.begin();
mem= new MEMTask("Memory",2048,queue,alarm_messages,web_messages,VERSION,AT24C32_ADDRESS,AT24C32_OFFSET);  
mem->resume();
leds = new LEDTask("Leds",3072,queue,leds_pins, HIGH);
leds->resume();
wifi=new WiFiTask("WiFi",4096,queue,flags);
wifi->resume();
http = new HTTP2Task("http",4096,queue,flags,web_messages);
http->resume();
rtc = new RTCTask("Clock",2048,flags,queue, display_message,alarm_messages);  
rtc->resume();
display= new DISPTask("Display",2048, display_message);  
display->resume();
relay= new RELTask("Relay",3072,queue,relays_pins);  
relay->resume();
// band= new BANDTask("Band",2048, queue,display_message, HIGH);  
// band->resume();
// btn= new BTNTask("Buttons",2048, queue);  
// btn->resume();
ir= new IRTask("IRс",3072,queue,IR_PIN,IR_DEVICE);  
ir->resume();

btn = new ENCTask("Encoder",2048,queue,ENCBTN,ENCS1,ENCS2,LOW);
btn->resume();



}

void web_event(event_t event){
  notify_t notify;
  switch (event.button){
    case ALARMSETUP://set alarm from web
     notify.title=event.button;
     notify.alarm=event.alarm;
     rtc->notify(notify);
  break;

  case ALARMSPRINT://print all alarms
  case ALARMACTIVEPRINT://print active alarm
  case ALARMSRESET://reset all alarms
     notify.title=event.button;
     notify.packet.var=0;
     notify.packet.value=0;
     rtc->notify(notify);
  break;
  
  case RELAYSET1:
  case RELAYSET2:
  case RELAYSET3:
  case RELAYSET4://set relays from web
    notify.title=event.button;
    notify.packet.var=0;//0 save 1 dont save
    notify.packet.value=event.count;
    relay->notify(notify);
  break;
      
      case LEDBRIGHTNESS1:
      case LEDBRIGHTNESS2:
      case LEDBRIGHTNESS3:
         notify.title=event.button;
         notify.packet.var=1;
         notify.packet.value=event.count;
         leds->notify(notify);
      break;
  }
}



void mem_event(event_t event){
  notify_t nt;
  switch (event.button){
   case RELWRITE1:
   case RELWRITE2:
   case RELWRITE3:
   case RELWRITE4://write relays (it works)
      nt.title=event.button;
      nt.packet.var=0;
      nt.packet.value=event.count;
      
      mem->notify(nt);
   break;
   case LEDWRITE1://save led state
   case LEDWRITE2:
   case LEDWRITE3:
   case LEDWRITE4:
      nt.title=event.button;
      nt.packet.var=event.count;
      nt.packet.value=event.data;
      mem->notify(nt);
   break;

   case 100:
   case 101:
   case 102:
   case 103:
   case 104:
   case 105:
   case 106:
   case 107:
   case 108:
   case 109:
        nt.title=event.button;
        nt.alarm=event.alarm;
        mem->notify(nt);
   break;
   case 199://request packed on WWW
      nt.title=199;
      nt.packet.var=0;
      nt.packet.value=0;
      mem->notify(nt);
   break;
   case 200://request packed in init from Mem
      nt.title=200;
      nt.packet.var=0;
      nt.packet.value=0;
      mem->notify(nt);
   break;
   case 201://reset and request packed reset
      nt.title=201;
      nt.packet.var=0;
      nt.packet.value=0;
      mem->notify(nt);
   break;
    case INITRELAYS://init relay devices from memory
      nt.title=INITRELAYS;
      nt.packet.var=0;
      nt.packet.value=event.data & 0xF;
      relay->notify(nt);
   break;
    case INITLEDS://init led`s devices from memory
      nt.title=LEDSETPARAM1;
      nt.packet.var=event.count & 0X000F;
      nt.packet.value=event.data & 0x000000FF;
      leds->notify(nt);
      vTaskDelay(pdMS_TO_TICKS(100));
      nt.title=LEDSETPARAM2;
      nt.packet.var=event.data >> 4 & 0x000F;
      nt.packet.value=event.data >> 8 & 0x000000FF;
      leds->notify(nt);
      vTaskDelay(pdMS_TO_TICKS(100));
      nt.title=LEDSETPARAM3;
      nt.packet.var=event.data   >> 8 & 0x000F;
      nt.packet.value=event.data >> 16 & 0x000000FF;
      leds->notify(nt);
      vTaskDelay(pdMS_TO_TICKS(100));
      nt.title=LEDSETPARAM4;
      nt.packet.var=event.data   >> 12 & 0x000F;
      nt.packet.value=event.data >> 24 & 0x000000FF;
      leds->notify(nt);
   break;
  }

}

void all_rel_off()
{
  
  notify_t nt; 
  nt.title=RELAYALLOFF;
  nt.packet.var=0;
  nt.packet.value=0;
  relay->notify(nt);
}

void all_leds_off()
{
  notify_t nt; 
  nt.title=LEDALLOFF;
  nt.packet.var=0;
  nt.packet.value=0;
  leds->notify(nt);
 }


void pult_event(event_t command)
{
      notify_t result;
      bool show_me = false;
      if ((command.count) == IR_DEVICE)
      {
    switch (command.button)
    {
    case PULT_1:
    case PULT_2:
    case PULT_3:
    case PULT_4:
     result.title = RELAYSWITCH1+command.button-PULT_1;
     relay->notify(result);

  
  break;
    case PULT_POWER: // all off
  all_rel_off();
  break;

  case PULT_FASTBACK:
  result.title = RTCGETTIME;
  rtc->notify(result);
  break;

  case PULT_PREV: // ultra low
   result.title=LEDBRIGHTNESSALL3;
   result.packet.value=8<<64 & 0xFF00|  0 & 0x00FF;
   result.packet.var=0; 
   leds->notify(result);
  break;
  case PULT_PAUSE: // low
    result.title=LEDBRIGHTNESSALL3;
   result.packet.value=8<<64 & 0xFF00|  64 & 0x00FF;
   result.packet.var=64; 
   leds->notify(result);
  break;
    case PULT_STOP: // middle
  result.title=LEDBRIGHTNESSALL3;
   result.packet.value=8<<128 & 0xFF00|  128 & 0x00FF;
   result.packet.var=128; 
   leds->notify(result);
  break;
   case PULT_NEXT: // full
  result.title=LEDBRIGHTNESSALL3;
   result.packet.value=0xFFFF;
   result.packet.var=0xFF; 
   leds->notify(result);
  break;
    default:
  show_me = true;
    }
      }
      if ((command.count) != IR_DEVICE || show_me)
      {
    String s = "EDevice " + String(command.count) + (command.count == IR_DEVICE ? "(MY)" : "(ALIEN)") + "*Command " + String(command.button) + "*type " + String(command.data);
    xMessageBufferSend(display_message, s.c_str(), s.length() + 1, portMAX_DELAY);
      }
}

void led_event(event_t command){
notify_t result;
switch (command.button)
    {
    case 111:
    case 112:
    case 113:
    result.title=command.button;
    result.packet.var=0;
    leds->notify(result);
    break;
    }
}

void button_event(event_t command){
notify_t result;
switch (command.button)
    {
    case 111:
    case 112:
    case 113:
    result.title=command.button;
    result.packet.var=0;
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
      button_event(command);
  break;
  case MEM_EVENT:
      mem_event(command);
  break;
  case DISP_EVENT:
      //display_event(command);
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