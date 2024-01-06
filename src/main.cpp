#include <Arduino.h>
#include "Settings.h"
#include <SPIFFS.h>
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
MEMTask<SystemState_t> * mem;
LEDTask * leds;
WiFiTask * wifi;
IRTask * ir;
HTTP2Task * http;
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
// mem= new MEMTask("Memory",2048,queue,alarm_messages,web_messages,VERSION,AT24C32_ADDRESS,AT24C32_OFFSET);  
// mem->resume();
mem=new MEMTask<SystemState_t>("MEM",2048,queue,reset_default,process_notify,web_messages,VERSION,AT24C32_ADDRESS,AT24C32_OFFSET);
mem->resume();

leds = new LEDTask("Leds",3072,queue,leds_pins, HIGH);
leds->resume();
wifi=new WiFiTask("WiFi",4096,queue,flags);
wifi->resume();
http = new HTTP2Task("http",4096,queue,flags,web_messages);
http->resume();
//rtc = new RTCTask("Clock",2048,flags,queue, display_message,alarm_messages);  
//rtc->resume();
rtc=new RTCTask("RTC",2048,flags,queue,web_messages);
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

//btn = new ENCTask("Encoder",2048,queue,ENCBTN,ENCS1,ENCS2,HIGH);
//btn->resume();

btn = new ENCTask("Encoder",1024,queue,ENCBTN,ENCS1,ENCS2,HIGH);
btn->resume();

gpio_set_pull_mode(ENCBTN, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(ENCS1, GPIO_PULLDOWN_ONLY);
gpio_set_pull_mode(ENCS2, GPIO_PULLDOWN_ONLY);

}

void web_event(event_t event){
  notify_t notify;
  String mess;
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
    mess="ARelay #";
    mess+=event.button-RELAYSET1-1;
    mess+=event.count?"*is set ON*":"*is set OFF*";
    xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
  break;
      
      case LEDBRIGHTNESS1:
      case LEDBRIGHTNESS2:
      case LEDBRIGHTNESS3:
         notify.title=event.button;
         notify.packet.var=1;
         notify.packet.value=event.count;
         leds->notify(notify);
         mess="ABrightness ";
         mess+=(notify.title==LEDBRIGHTNESS1?"CW":notify.title==LEDBRIGHTNESS2?"NW":"WW");
         mess+=event.count?"*is set ":"is set ";
         mess+=event.count;
         mess+="*";
        xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
      break;
  }
}



void mem_event(event_t event){
  notify_t nt;
  switch (event.button){
   case MEM_SAVE_00:  
   case MEM_SAVE_01:
   case MEM_SAVE_02:
   case MEM_SAVE_03:
   case MEM_SAVE_04:
   case MEM_SAVE_05:
   case MEM_SAVE_06:
   case MEM_SAVE_07:
   case MEM_SAVE_08:
   case MEM_SAVE_09:
   nt.title=event.button;
   nt.alarm=event.alarm;
   mem->notify(nt);
 break;  
   case MEM_SAVE_10:
   case MEM_SAVE_11:
   case MEM_SAVE_12:
   case MEM_SAVE_13:
   nt.title=event.button;
   nt.packet.value=event.data;
   mem->notify(nt);
   if (event.count==1){//инициатор процесс
      http->notify(nt);
   }else{//инициатор веб
      nt.title=event.button-150;
      nt.packet.value=event.data;
      relay->notify(nt); 
   }
  break; 

   case MEM_SAVE_14:
   case MEM_SAVE_15:
   case MEM_SAVE_16:
   nt.title=event.button;
   nt.packet.value=event.data;
   mem->notify(nt);
   if (event.count==1){//инициатор процесс
      http->notify(nt);
   }else{//инициатор веб
      nt.title=event.button-150;
      nt.packet.value=event.data;
      leds->notify(nt); 
   }
  break; 

    case MEM_ASK_00:
    case MEM_ASK_01:
    case MEM_ASK_02:
    case MEM_ASK_03:
    case MEM_ASK_04:
    case MEM_ASK_05:
    case MEM_ASK_06:
    case MEM_ASK_07:
    case MEM_ASK_08:
    case MEM_ASK_09:
    case MEM_ASK_10:
    case MEM_ASK_11:
    case MEM_ASK_12:
    case MEM_ASK_13:
    case MEM_ASK_14:
    case MEM_ASK_15:
    case MEM_ASK_16:
      nt.title=event.button;
      mem->notify(nt);
    break;

    case MEM_READ_00:
    case MEM_READ_01:
    case MEM_READ_02:
    case MEM_READ_03:
    case MEM_READ_04:
    case MEM_READ_05:
    case MEM_READ_06:
    case MEM_READ_07:
    case MEM_READ_08:
    case MEM_READ_09:
    nt.title=ALARMSETFROMMEM;
    nt.alarm=event.alarm;
    rtc->notify(nt);
    break;
    case MEM_READ_10:
    case MEM_READ_11:
    case MEM_READ_12:
    case MEM_READ_13:
    nt.title=event.button;
    nt.packet.value=event.data;
    relay->notify(nt);
    if (event.count==1){//инициатор память
      nt.title=event.button+150;
      http->notify(nt);
    }

    case MEM_READ_14:
    case MEM_READ_15:
    case MEM_READ_16:
    nt.title=event.button;
    nt.packet.value=event.data;
    leds->notify(nt);
    if (event.count==1){//инициатор память
      nt.title=event.button+150;
      http->notify(nt);
    }
    break;

  }

}

void all_rel_off()
{
  String mess;
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
      String mess;
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
    mess="ARelay #";
    mess+=command.button;
    mess+="*SWITCHED*";
    xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
  
  break;
  case PULT_5:
  // pinMode(GPIO_NUM_2,OUTPUT);
  // digitalWrite(GPIO_NUM_2,!digitalRead(GPIO_NUM_2));
  //  result.title=LEDMODE4;
  //  result.packet.value=BLINK_FADEINOUT;
  //  result.packet.var=0; 
  //  leds->notify(result);
  //  vTaskDelay(pdMS_TO_TICKS(100));
  //  result.title=LEDBRIGHTNESS4;
  //  result.packet.value=128;
  //  result.packet.var=0; 
  //  leds->notify(result);
   
      //Serial.print("Mode");
      
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
   result.packet.value=8<<0 & 0xFF00|  0 & 0x00FF;
   result.packet.var=64; 
   leds->notify(result);
   mess="ALifgt*is set to*ULTRA LOW";
   xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
   result.title=RELAYSET3;
   result.packet.value=1;
   result.packet.var=0;//need to save
   relay->notify(result);
  break;  
  case PULT_PAUSE: // low
   result.title=LEDBRIGHTNESSALL3;
   result.packet.value=(0x40<<8) & 0xFF00 |  0x40 & 0x00FF;
   result.packet.var=0x40; 
   leds->notify(result);
   mess="ALifgt*is set to*LOW";
   xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
   result.title=RELAYSET3;
   result.packet.value=1;
   result.packet.var=0;//need to save
   relay->notify(result);
  break;
    case PULT_STOP: // middle
  result.title=LEDBRIGHTNESSALL3;
   result.packet.value=(0x80<<8) & 0xFF00 |  0x80 & 0x00FF;
   result.packet.var=0x80; 
   mess="ALifgt*is set to*MIDDLE";
   xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
   leds->notify(result);
   result.title=RELAYSET3;
   result.packet.value=1;
   result.packet.var=0;//need to save
   relay->notify(result);
  break;
   case PULT_NEXT: // full
  result.title=LEDBRIGHTNESSALL3;
   result.packet.value=0xFFFF;
   result.packet.var=0xFF; 
   leds->notify(result);
   mess="ALifgt*is set to*MAXIMUM";
   xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
   result.title=RELAYSET3;
   result.packet.value=1;
   result.packet.var=0;//need to save
   relay->notify(result);
  break;
    default:
  show_me = true;
    }
      }
      if ((command.count) != IR_DEVICE || show_me)
      {
        mess = "EDevice " + String(command.count) + (command.count == IR_DEVICE ? "(MY)" : "(ALIEN)") + "*Command " + String(command.button) + "*type " + String(command.data);
        xMessageBufferSend(display_message, mess.c_str(), mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(), portMAX_DELAY);
      }
}

void led_event(event_t command){
notify_t result;
result.title=0;
switch (command.button)
    {
    case LED_CONNECTING:
      result.title=LEDMODE4;
      result.packet.value=(uint16_t)BLINK_4HZ;
      result.packet.var=0; 
      leds->notify(result);
      break;
    case LED_CONNECTED:
      result.title=LEDMODE4;
      result.packet.value=(uint16_t)BLINK_FADEINOUT;
      result.packet.var=0; 
      leds->notify(result);
      break;
    case LED_CONNECT_FAILED:
      result.title=LEDMODE4;
      result.packet.value=(uint16_t)BLINK_OFF;
      result.packet.var=0; 
      leds->notify(result);
    break;
    }
    if (result.title){
      result.title=LEDBRIGHTNESS4;
      result.packet.value=128;
      result.packet.var=0; 
      vTaskDelay(pdMS_TO_TICKS(100));
      leds->notify(result);
    }
}

void button_event(event_t command){
notify_t result;
switch (command.button)
{
    case 0:
    switch (command.state)
    {
    case BTN_CLICK:
    // Serial.print("Count:");
    // Serial.println(command.count);
    if (command.count==1){
      result.title=RTCGETTIME;
      rtc->notify(result);
    }
    
    break;
    case BTN_LONGCLICK:
    Serial.print("Long click after ");
    Serial.println(command.count);
    break;
    
    }
    break;
    case 1:
    break;
}
}
void display_event(event_t event){
  String mess;
  switch (event.button)
  {
  case SHOWTIME:
    mess="QTime:";
    mess+=event.alarm.hour;
    mess+=":";
    mess+=event.alarm.minute<10?"0":"";
    mess+=event.alarm.minute;
    mess+="*Date:";
    mess+=(uint8_t)event.alarm.period;
    mess+="/";
    mess+=event.alarm.action<10?"0":"";
    mess+=event.alarm.action;
    mess+="*";
    mess+=dayofweek[(event.alarm.wday*5)];
    mess+=dayofweek[(event.alarm.wday*5)+1];
    mess+=dayofweek[(event.alarm.wday*5)+2];
    mess+=dayofweek[(event.alarm.wday*5)+3];
    mess+=dayofweek[(event.alarm.wday*5)+4];
    xMessageBufferSend(display_message, mess.c_str(), mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(), portMAX_DELAY);
    
    break;
  
  default:
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