#include "RTCTask.h"
//#include "Events.h"

void RTCTask::setup()
{
rtc= new RTC_DS3231();
rtc->begin();
fast_time_interval=true;
last_sync=0;
vTaskDelay(pdMS_TO_TICKS(5000));
initAlarms();
init_complete=false;
}

void RTCTask::initAlarms(){
event_t e;
e.state=MEM_EVENT;
e.button=200;
xQueueSend(que,&e,portMAX_DELAY);
}

void RTCTask::resetAlarms(){
  event_t e;
  e.state=MEM_EVENT;
  e.button=201;
  init_complete=false;
  xQueueSend(que,&e,portMAX_DELAY);
}



void RTCTask::cleanup()
{
delete rtc;
}

//установка конкретного будильника
void RTCTask::alarm(alarm_t &a){
  a.active=true;
  DateTime dt;
  
  switch (a.period)
  {
    case  ONCE_ALARM:
    case  EVERYHOUR_ALARM:
    case  EVERYDAY_ALARM:
      rtc->setAlarm2(dt+TimeSpan(0,a.hour,a.minute,0),DS3231_A2_Hour);  
    case  WDAY_ALARM:
    case  HDAY_ALARM:
    case  WD1_ALARM:
    case  WD2_ALARM:
    case  WD3_ALARM:
    case  WD4_ALARM:
    case  WD5_ALARM:
    case  WD6_ALARM:
    case  WD7_ALARM:
      rtc->setAlarm2(dt+TimeSpan(a.wday+1>6?0:a.wday+1,a.hour,a.minute,0),DS3231_A2_Day);
    break;
  }
}





void RTCTask::alarmFired(uint8_t an){

DateTime dt;
if (an==1){
//Serial.println("Alarm 1 fired");
rtc->clearAlarm(1);
}else if(an==2){
dt=rtc->getAlarm2();
rtc->clearAlarm(2);
event_t ev;
uint8_t idx=findAndSetNext(dt,rtc->getAlarm2Mode());
if (idx<ALARMS_COUNT){
ev.state=RTC_EVENT;
ev.button=alarms[idx].action;
ev.alarm=alarms[idx];
xQueueSend(que,&ev,portMAX_DELAY);
idx=refreshAlarms();
}
}
}

//find fired alarm and set for this alarm next period
uint8_t RTCTask::findAndSetNext(DateTime dt, Ds3231Alarm2Mode mode){
  uint8_t result=ALARMS_COUNT;
for (int i=0;i<ALARMS_COUNT;i++){
  if (!alarms[i].active) continue;
  
  if (alarms[i].hour==dt.hour() && alarms[i].minute==dt.minute()){
    
    if (mode==DS3231_A2_Hour){
      if (alarms[i].period==EVERYHOUR_ALARM || alarms[i].period==EVERYDAY_ALARM || alarms[i].period==ONCE_ALARM) {result=i;break;}
    }
    else{
      if (dt.dayOfTheWeek()==alarms[i].wday){result=i;break;}
    }

  }
}
if (result<ALARMS_COUNT) 
{
getNext(alarms[result]);
saveAlarm(result);
}

return result;
}

void RTCTask::saveAlarm(uint8_t idx){
event_t ev;
ev.state=MEM_EVENT;
ev.button=idx+100;
ev.alarm=alarms[idx];
xQueueSend(que,&ev,portMAX_DELAY);
}

bool RTCTask::setupAlarm(uint8_t idx, uint8_t act, uint8_t h, uint8_t m,  period_t p){
if (idx>=ALARMS_COUNT) return false;
alarms[idx].active=true;
alarms[idx].action=act;
alarms[idx].minute=m;
alarms[idx].period=p;


DateTime dt=rtc->now();
uint16_t amin=h*60+m;
uint16_t nmin=dt.hour()*60+dt.minute();
uint8_t dw=dt.dayOfTheWeek();

if (p>=WD7_ALARM) {dw=(uint8_t)p-(uint8_t)WD7_ALARM;}
else if (p==WDAY_ALARM) {
if (dw>5||dw==0) {dw=1;} 
else if (nmin>=amin) {dw=dw<5?dw+1:1;}
}else if (p==HDAY_ALARM) {
if (dw<6 && dw!=0) {dw=6;} 
else if (nmin>=amin) {dw=dw==6?0:6;}
}else if (p==ONCE_ALARM || p==EVERYDAY_ALARM){

}else if (p==EVERYHOUR_ALARM){
  h=dt.hour();
  if(m<=dt.minute()) h++;
  if (h>23) h=0;
}

alarms[idx].hour=h;
alarms[idx].wday=dw;
saveAlarm(idx);
return true;
}


//find and set next nearest alarm
uint8_t RTCTask::refreshAlarms(){
rtc->clearAlarm(1);
rtc->clearAlarm(2);
uint8_t index=ALARMS_COUNT;
uint16_t amin,nmin,cdiff,min_diff=WEEK+1;//week and one minutes
DateTime d=rtc->now();
for (uint8_t i=0;i<ALARMS_COUNT;i++){
  if (!alarms[i].active) continue;
  amin=alarms[i].hour*60+alarms[i].minute;
  nmin=d.hour()*60+d.minute();
  switch (alarms[i].period)
  {
  case EVERYHOUR_ALARM:
    while(amin<=nmin) amin+=60;
  break;
  case EVERYDAY_ALARM:
  case ONCE_ALARM:
  if (amin<=nmin) amin+=DAY;
    break;
  default:
  amin+=(alarms[i].wday*DAY+((d.dayOfTheWeek()>alarms[i].wday)?WEEK:0));
  nmin+=d.dayOfTheWeek()*DAY;
  if (amin<=nmin) amin+=WEEK;
    break;
  }
cdiff=amin-nmin;  
#ifdef DEBUGG
Serial.printf("Diff=%d ", cdiff);
Serial.print(printAlarm(alarms[i]).c_str());
#endif
if (cdiff<min_diff){
  min_diff=cdiff;
  index=i;
}
}
if (index<ALARMS_COUNT) {
alarm(alarms[index]);
}
return index;
}

void RTCTask::loop()
{

     
    //if (xMessageBufferReceive(alarm_mess,&ss,SSTATE_LENGTH,!init_complete?portMAX_DELAY:0)==SSTATE_LENGTH){
      SystemState_t sst;
      if (xMessageBufferReceive(alarm_mess,&sst,SSTATE_LENGTH,!init_complete?pdMS_TO_TICKS(1000):0)==SSTATE_LENGTH){
      memcpy(alarms,sst.alr,sizeof(alarm_t)*ALARMS_COUNT);
      refreshAlarms();  
      init_complete=true;
      event_t ev;
      ev.state=MEM_EVENT;//init other devices
      ev.button=203;
      ev.data=sst.rel[0] & 1 | sst.rel[1]<<1 & 2 | sst.rel[2]<<2 & 4 | sst.rel[3]<<3 & 8; 
      xQueueSend(que,&ev,portMAX_DELAY);
    }
      
    uint32_t command;
    notify_t nt;   
    if (xTaskNotifyWait(0, 0, &command, init_complete?pdMS_TO_TICKS(1000):0))
    {
         
        memcpy(&nt,&command,sizeof(command));
        switch (nt.title)
        {
        case 1:
        
          setupAlarm(nt.alarm.action,nt.alarm.action,nt.alarm.hour,nt.alarm.minute,nt.alarm.period);
          #ifdef DEBUGG
          portENTER_CRITICAL(&_mutex);
          Serial.print(printAlarm(alarms[nt.alarm.action]).c_str());   
          portEXIT_CRITICAL(&_mutex);
        #endif
          refreshAlarms();
          break;
        case 10:{
            char buf[100];
            size_t si;
            int res;
            if (fast_time_interval){
              res = snprintf(buf, sizeof(buf), "%s","Time is not*syncronized**");
            }else{
              DateTime dt=rtc->now();
              char p [3];
              strncpy(p,dayofweek+3 * dt.dayOfTheWeek(),3);
              res = snprintf(buf, sizeof(buf), "Time %d:%02d:%02d*Date %d-%02d-%d*%s", dt.hour(),dt.minute(),dt.second(),dt.day(),dt.month(),dt.year(),p);
            }
            si=xMessageBufferSend(disp_mess,buf,res,portMAX_DELAY);
            break;}
        case 11:
        {
          #ifdef DEBUGG
            for (uint8_t ii=0;ii<ALARMS_COUNT;ii++){
              
                Serial.print(printAlarm(alarms[ii]).c_str());
            }
            #endif
        break;  
        }
        case 12:{
        DateTime dtm=rtc->getAlarm2();
        #ifdef DEBUGG
              Serial.printf("Active alarm=%02d:%02d Wday=%d\n",dtm.hour(),dtm.minute(),dtm.dayOfTheWeek());
              #endif
        break;   

        } 
        case 13:
        #ifdef DEBUGG
          Serial.print("Reset alarms");
          #endif
          resetAlarms();
          refreshAlarms();
          
        break;  
     }
    }
if (rtc->alarmFired(1)){
alarmFired(1);
}
if (rtc->alarmFired(2)){
alarmFired(2);
}  


//if (xEventGroupWaitBits(flg, FLAG_WIFI, pdFALSE, pdTRUE, portMAX_DELAY) & FLAG_WIFI) {    
if (xEventGroupWaitBits(flg, FLAG_WIFI, pdFALSE, pdTRUE,pdMS_TO_TICKS(1000)) & FLAG_WIFI) {    
unsigned long t= millis();    
if (t < last_sync) last_sync=t;
  if (last_sync==0 || t - last_sync > (fast_time_interval ? SHORT_TIME : LONG_TIME))
  {
    last_sync = t;
    fast_time_interval = !update_time_from_inet();
  }
}

}

bool RTCTask::update_time_from_inet()
{
  WiFiUDP *ntpUDP;
  NTPClient *timeClient;
  ntpUDP = new WiFiUDP();

  timeClient = new NTPClient(*ntpUDP, NTPServer, 3600 * TIME_OFFSET, 60000 * 60 * 24);
  timeClient->begin();
  bool result=timeClient->forceUpdate();
  if (result)
  {
    DateTime d(timeClient->getEpochTime());
    rtc->adjust(d);
    #ifdef DEBUGG
    Serial.println("Success update time from inet. Time is :" + rtc->now().timestamp());
    #endif
  }
    

  timeClient->end();
  delete timeClient;
  delete ntpUDP;
  return result;
}