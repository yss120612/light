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
    sleeping_time=millis()+time;
}

uint32_t IRreceiver::getCommand(){
    return last_command;
}

uint32_t IRreceiver::getDevice(){
    return last_device;
}
uint32_t IRreceiver::checkIR(unsigned long ms){
  if (sleeping_time!=0 && ms>sleeping_time)  {
      enable();
  }
  if (irrecv->decode(&dres))
  {
      
     //if (dres.command>0 && dres.address==IR_DEVICE){
     if (dres.command>0){
        if (ms-time_lc>PERIOD || dres.command!=last_command)
        {  
        last_command=dres.command;
        last_device=dres.address;
        time_lc=ms;
        //logg.logging("IR Command="+String(last_command));
        return dres.command;
        }
      }
      else if(dres.command>0){
          last_device=dres.address;
          last_command=dres.command;
          return dres.command;
          //logg.logging("Not my device ("+String(IR_DEVICE)+")! Device="+String(dres.address)+";IR Command="+String(dres.command));
      }
      irrecv->resume();
  }
  return 0;
  }

