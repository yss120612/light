#include "IRreceiver.h"
#include "Log.h"
#include "Settings.h"

IRreceiver::IRreceiver(uint16_t port){
    _port=port;
    irrecv = new IRrecv(_port);
    last_command=0;
    time_lc=0;
    sleeping_time=0;
    
}

IRreceiver::~IRreceiver(){
    delete irrecv;
}

void IRreceiver::enable(){
    irrecv->enableIRIn();
    sleeping_time=0;
}

void IRreceiver::disable(){
    irrecv->disableIRIn();
}

void IRreceiver::sleep_sometime(unsigned long time){
    disable();
    ulong m=millis();
    sleeping_time=(m+time)<time?time:m+time;
}

uint32_t IRreceiver::getCommand(){
    return last_command;
}

void IRreceiver::resetCommand(){
    last_command=0;
}

uint32_t IRreceiver::getDevice(){
    return last_device;
}

decode_type_t IRreceiver::getType(){
    return (decode_type_t)last_type;
}


uint32_t IRreceiver::checkIR(unsigned long ms){
  bool sig=false;  
  if (sleeping_time!=0 && ms>sleeping_time)  {
      enable();
  }

  if (irrecv->decode(&dres))
  {
      
     //if (dres.command>0 && dres.address==IR_DEVICE){
    //  if (dres.command>0){
    //     if (ms-time_lc>PERIOD || dres.command!=last_command)
    //     {  
    //     last_command=dres.command;
    //     last_device=dres.address;
    //     last_type=dres.decode_type;
    //     time_lc=ms;
    //logg.logging("IR Command="+String(last_command));
    //     return dres.command;
    //     }
    //   }
    //   else 
      if(dres.command>0 && (ms-time_lc>PERIOD || dres.command!=last_command)){
          last_device=dres.address;
          last_command=dres.command;
          last_type=dres.decode_type;
          sig=true;
          logg.logging("Delta="+String(ms-time_lc)+" Command="+String(last_command) +" Dres command=="+String(dres.command));
       
      }
      
      time_lc=ms;
      irrecv->resume();
      
  }
   return sig?dres.command:0;
  }

