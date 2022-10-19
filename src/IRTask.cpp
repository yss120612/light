#include "IRTask.h"
#include "Events.h"

void IRTask::setup(){
irrecv = new IRrecv(IRPIN);
irrecv->enableIRIn();
irrecv->resume();
}

void IRTask::loop(){
    
    if (irrecv->decode(&dres))
{       uint32_t command=dres.command;
        if (command){
        lock();
        event_t ev;
        ev.state=PULT_BUTTON;
        ev.button=(uint8_t)dres.command;
        ev.count=(uint8_t)dres.address;
        ev.type=(int8_t)dres.decode_type;
        Serial.print("Command="); 
        Serial.print(ev.button);
        Serial.print(" Address="); 
        Serial.print(ev.count);
        Serial.print(" Type="); 
        Serial.println(ev.type);
        xQueueSend(que,&ev,portMAX_DELAY);
        unlock();
        }
        irrecv->resume();
    }
        
    delay(100);
}

void IRTask::cleanup(){
    irrecv->disableIRIn();
    delete irrecv;
}