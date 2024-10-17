//#define DEBUGG
static const char * TAG="Light";

#include <esp_log.h>
#include <driver\i2c.h>
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
//#include <RTCLib.h>
//#include "HTTP2Task.h"
#include "HTTPServer.h"
#include "MAX7219Task.h"
#include "WeatherTask.h"
#include "TelegramTask.h"


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
State state;
QueueHandle_t queue;
SemaphoreHandle_t btn_semaphore;
EventGroupHandle_t flags;
MessageBufferHandle_t display_message,tablo_messages,web_messages,telega_messages;

//Blinker * blinker;
MEMTask<SystemState_t> * mem;
LEDTask * leds;
WiFiTask * wifi;
IRTask * ir;
//HTTP2Task * http;
Display * display;
RTCTask * rtc;
RELTask * relay;
//BANDTask * band;
ENCTask * btn;
HTTPServer * http;

Max7219Task * max7219;
WeatherTask * weather;
TelegramTask * telega;

//extern void init_networks();

// StaticQueue_t queueBuffer;
// uint8_t queueStorage[QUEUE_LENGTH * sizeof(event_t)];

// static void WiFiTaskHandler(void *pvParam) {
//   const uint32_t WIFI_TIMEOUT = 30000; // 30 sec.
// }


void setup() {
  
  	
  

//vTaskDelay(pdMS_TO_TICKS(2000));

 //queue= xQueueCreate(16,sizeof(event_t));
 //queue= xQueueCreateStatic(QUEUE_LENGTH,sizeof(event_t),&queueStorage[0],&queueBuffer);
 queue= xQueueCreate(QUEUE_LENGTH,sizeof(event_t));
 //btn_semaphore=xSemaphoreCreateBinary();
 flags=xEventGroupCreate();
 xEventGroupClearBits(flags,0xFFFF);
 display_message=xMessageBufferCreate(DISP_MESSAGE_LENGTH+4);
 if (!display_message){
  _LOG(TAG,"Error creating display message!");
 }
 telega_messages=xMessageBufferCreate(100+4);
 if (!telega_messages){
  _LOG(TAG,"Error creating telega_messages!");
 }

 tablo_messages=xMessageBufferCreate(sizeof(event_t)+4);//=length label
 //web_messages=xMessageBufferCreate(SSTATE_LENGTH+4);
#ifdef DEBUGG
   Serial.begin(115200);
#endif

 //esp_log_level_set("*",ESP_LOG_INFO);
  
  i2c_config_t i2c_config;
	i2c_config.mode=I2C_MODE_MASTER;
	i2c_config.scl_io_num=SCL;
	i2c_config.sda_io_num=SDA;
	i2c_config.scl_pullup_en=false;
	i2c_config.sda_pullup_en=false;
	i2c_config.master.clk_speed=100000;
	i2c_config.clk_flags=I2C_SCLK_SRC_FLAG_FOR_NOMAL;

	esp_err_t error=i2c_param_config(0,&i2c_config);
	if (error!=ESP_OK){
		_LOG(TAG,"Error config i2c");
		return;
	}
	error=i2c_driver_install(0,I2C_MODE_MASTER,0,0,0);
	if (error!=ESP_OK){
		_LOG(TAG,"Error installing i2c");
		return;
	}


  
//vTaskDelay(pdMS_TO_TICKS(2000));

mem=new MEMTask<SystemState_t>("MEM",2048,queue,reset_default,process_notify,&(state.st),VERSION,AT24C32_ADDRESS,AT24C32_OFFSET);
mem->resume();

//display= new DISPTask("Display",2048, display_message);  

ir= new IRTask("IRс",2048,queue,IR_PIN,IR_DEVICE);  


// btn = new ENCTask("Encoder",2048,queue,ENCBTN,ENCS1,ENCS2,HIGH);
// btn->resume();

 wifi=new WiFiTask("WiFi",4096,queue,flags);

//http = new HTTP2Task("http",4096,queue,flags,web_messages);
//http->resume();

http = new HTTPServer("http",4096,flags,queue,&(state.st));

rtc=new RTCTask("RTC",2048,queue);

relay= new RELTask("Relay",1024,queue,relays_pins);  

leds = new LEDTask("Leds",1024,queue,leds_pins, HIGH);

max7219= new Max7219Task("Watch",2048,flags,queue,tablo_messages);

weather= new WeatherTask("Weather",1024*3,queue,flags);

// uint32_t rw=esp_random();
// bool run_weather=rw%2;

//if (run_weather){
//_LOG(TAG,"RUN TELEGRAM %d",rw);
telega= new TelegramTask("Telegram",1024*4,queue,flags,telega_messages);

//}

//gpio_set_pull_mode(ENCBTN, GPIO_PULLDOWN_ONLY);
//gpio_set_pull_mode(ENCS1, GPIO_PULLDOWN_ONLY);
//gpio_set_pull_mode(ENCS2, GPIO_PULLDOWN_ONLY);
//ESP_LOGE(TAG,"size notify=%d uint32_t=%d",sizeof(notify_t),sizeof(uint32_t));
//vTaskDelay(pdMS_TO_TICKS(2000));
_LOG(TAG,"Ready!");

ir->resume();
rtc->resume();
relay->resume();
leds->resume();
wifi->resume();
http->resume();
//display->resume();
max7219->resume();
//if (run_weather){
weather->resume();
//}
rtc->needWatch();//хочу часы
//http->start();
//all_leds_off();
 //state.relays[3].ison=true;
}

void web_event(event_t event){
  notify_t notify;
  //String mess;
   
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
    event.count = event.button-RELAYSET1+1;
    event.button = 1;
    xMessageBufferSend(tablo_messages, &event, sizeof(event_t), 0);
    // mess="ARelay #";
    // mess+=event.button-RELAYSET1-1;
    // mess+=event.count?"*is set ON*":"*is set OFF*";
    // xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
  break;
      
      case LEDBRIGHTNESS1:
      case LEDBRIGHTNESS2:
      case LEDBRIGHTNESS3:
         notify.title=event.button;
         notify.packet.var=1;
         notify.packet.value=event.count;
         leds->notify(notify);
        //  mess="ABrightness ";
        //  mess+=(notify.title==LEDBRIGHTNESS1?"CW":notify.title==LEDBRIGHTNESS2?"NW":"WW");
        //  mess+=event.count?"*is set ":"is set ";
        //  mess+=event.count;
        //  mess+="*";
        // xMessageBufferSend(display_message,mess.c_str(),mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(),100);
      break;
  }
}

void mem_event(event_t event){
  notify_t nt;
 
  switch (event.button){
   case ASK_ALL:
    nt.title=event.button;
    mem->notify(nt);
   break; 
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
      //http->notify(nt);
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
      //http->notify(nt);
   }else{//инициатор веб
      nt.title=event.button-150;
      nt.packet.value=event.data;
      //leds->notify(nt); 
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
     //_LOG(TAG,"Relay notify %d",nt.packet.value);
    //  if (event.count==1){//инициатор память
    //    nt.title=event.button+150;
    //  }

    case MEM_READ_14:
    case MEM_READ_15:
    case MEM_READ_16:
    nt.title=event.button;
    nt.packet.value=event.data;
    leds->notify(nt);
    // if (event.count==1){//инициатор память
    //   nt.title=event.button+150;
    // }
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
   // if ((command.count) == IR_DEVICE)
   if ((command.count) > 0)
   {
     switch (command.button)
     {
     case PULT_1:
       result.title = RELAYSWITCH1;
       relay->notify(result);
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_2:
       result.title = RELAYSWITCH2;
       relay->notify(result);
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_3:
       result.title = RELAYSWITCH3;
       relay->notify(result);
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_4:
       result.title = RELAYSWITCH4;
       relay->notify(result);
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_5:
       result.title = LEDBRIGHTNESSALL3;
       // LO value=WW  HI value=NW, var=CW
       result.packet.value = 0x0080;
       result.packet.var = 00;
       leds->notify(result);
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_6:
       result.title = LEDBRIGHTNESSALL3;
       result.packet.value = 0x8080;
       result.packet.var = 0x80;
       leds->notify(result);
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_7:
       result.title = LEDBRIGHTNESSALL3;
       result.packet.value = 0xFFFF;
       result.packet.var = 0xFF;
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       leds->notify(result);
       break;

     case PULT_8:
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       all_rel_off();
       break;

     case PULT_9:
       command.count = command.button;
       command.button = 1;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       all_leds_off();
       break;
    case PULT_PREV:
       result.title = LEDBRIGHTNESS1;
       result.packet.value = 0xFF;
       result.packet.var = 0;
       leds->notify(result);
       break;
       case PULT_PAUSE:
       result.title = LEDBRIGHTNESS2;
       result.packet.value = 0xFF;
       result.packet.var = 0;
       leds->notify(result);
       break;
       case PULT_STOP:
       result.title = LEDBRIGHTNESS3;
       result.packet.value = 0xFF;
       result.packet.var = 0;
       leds->notify(result);
       break;
     case PULT_FASTBACK:
       command.count = 3; // brightness ++
       command.button = 10;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_PLAY:
       command.count = 1; // brightness default
       command.button = 10;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     case PULT_FASTFORWARD:
       command.count = 2; // brightness --
       command.button = 10;
       xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
       break;
     default:
       show_me = true;
     }
   }
   if ((command.count) != IR_DEVICE || show_me)
   {
     
     command.data=(command.count << 8) & 0xFF00 | command.button & 0xFF;
     command.count = 21;
     command.button = 1;
     xMessageBufferSend(tablo_messages, &command, sizeof(event_t), 0);
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
      //leds->notify(result);
      break;
    case LED_CONNECT_FAILED:
      result.title=LEDMODE4;
      result.packet.value=(uint16_t)BLINK_OFF;
      result.packet.var=0; 
      //leds->notify(result);
    break;
    }
    if (result.title){
      result.title=LEDBRIGHTNESS4;
      result.packet.value=128;
      result.packet.var=0; 
      vTaskDelay(pdMS_TO_TICKS(100));
      //leds->notify(result);
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
    //Serial.print("Long click after ");
    //Serial.println(command.count);
    break;
    
    }
    break;
    case 1:
    break;
}
}

void rtc_event(event_t event){
  notify_t result;

  switch (event.button)
  {
    case RTCTIMEADJUST:
      result.title=RTCTIMEADJUST;
      result.packet.var=0;
      result.packet.value=(event.data>>16) & 0xFFFF;
      rtc->notify(result);
      vTaskDelay(pdMS_TO_TICKS(500));
      result.packet.var=1;
      result.packet.value=event.data  & 0xFFFF;
      rtc->notify(result);
    break;
  }
}

void alarm_event(event_t event){
  //uint8_t alr=event.button>>24 & 0xF;  
event_t sevent;
String s;
switch(event.button)
{
    case 0:
    break;
    case 1:
     all_leds_off();
    break;
    case 2:
    break;
    case 3:
    break;
    case 4:
    break;
    case 5:
    break;
    case 6:
    break;
    case 7:
    break;
    case 8:
    break;
    case 9:
      xMessageBufferSend(tablo_messages,&event,sizeof(event_t),0);
      //s=String(event.alarm.minute);
    break;
    case 10:
      event.button=2;
      ESP_LOGI(TAG,"METEODATA=%X",event.data);
      xMessageBufferSend(tablo_messages,&event,sizeof(event_t),0);
    break;
}
}

void display_event(event_t event){
  String mess;
  switch (event.button)
  {
  case SHOWTIME:
    // mess="QTime:";
    // mess+=event.alarm.hour;
    // mess+=":";
    // mess+=event.alarm.minute<10?"0":"";
    // mess+=event.alarm.minute;
    // mess+="*Date:";
    // mess+=(uint8_t)event.alarm.period;
    // mess+="/";
    // mess+=event.alarm.action<10?"0":"";
    // mess+=event.alarm.action;
    // mess+="*";
    // mess+=dayofweek[(event.alarm.wday*4)];
    // mess+=dayofweek[(event.alarm.wday*4)+1];
    // mess+=dayofweek[(event.alarm.wday*4)+2];
    // mess+=dayofweek[(event.alarm.wday*4)+3];
    // //mess+=dayofweek[(event.alarm.wday*7)+4];
    // xMessageBufferSend(display_message, mess.c_str(), mess.length()>DISP_MESSAGE_LENGTH?DISP_MESSAGE_LENGTH:mess.length(), portMAX_DELAY);
    
    break;
  
  default:
    break;
  }
}

void loop()
{

event_t command;
if (xQueueReceive(queue,&command,portMAX_DELAY)==pdTRUE)
{
  


//return;
  //_LOG(TAG,"loopI command=%d button=%d",command.state,command.button);
  //ESP_LOGI(TAG,"loopE command=%d button=%d",command.state,command.button);
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
  case RTC_EVENT:
      rtc_event(command);
  break;
  case ALARM_EVENT:
      alarm_event(command);
  break;
 }
}//if queue
else{
  _LOG(TAG,"error receive queue");
}
vTaskDelay(pdMS_TO_TICKS(10));
}