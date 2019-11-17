#include "IRreceiver.h"
#include "Log.h"
#include "Settings.h"

IRreceiver::IRreceiver(uint16_t port){
    _port=port;
    irrecv = new IRrecv(_port);
    last_command=0;
    time_lc=0;
}

IRreceiver::~IRreceiver(){
    delete irrecv;
}

void IRreceiver::enable(){
    irrecv->enableIRIn();
}

void IRreceiver::disable(){
    irrecv->disableIRIn();
}

uint32_t IRreceiver::getCommand(){
    return last_command;
}

uint32_t IRreceiver::checkIR(unsigned long ms){
  if (irrecv->decode(&dres))
  {
      if (dres.command>0 && dres.address==IR_DEVICE){
        if (ms-time_lc>PERIOD || dres.command!=last_command)
        {  
        last_command=dres.command;
        time_lc=ms;
        logg.logging("IR Command="+String(last_command,0));
        return dres.command;
        }
      }
      irrecv->resume();
  }
  return 0;
  }

