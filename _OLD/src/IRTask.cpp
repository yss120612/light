#include "IRTask.h"


void IRTask::setup(){
irrecv = new IRrecv(IRPIN);
irrecv->enableIRIn();
irrecv->resume();
}

void IRTask::loop(){
    
    if (irrecv->decode(&dres))
{       uint32_t command=dres.command;
        if (command && command!=old_command){
        old_command=command;    
        event_t ev;
        ev.state=PULT_BUTTON;
        ev.button=(uint8_t)dres.command;
        ev.count=(uint8_t)dres.address;
        ev.data=dres.decode_type<0?999:(int32_t)dres.decode_type;
        Serial.print("IR Command="); 
        Serial.print(ev.button);
        Serial.print(" Address="); 
        Serial.print(ev.count);
        Serial.print(" Type="); 
        Serial.println(ev.data);
        xQueueSend(que,&ev,portMAX_DELAY);
        
        }
        irrecv->resume();
    }
        
    delay(500);
    old_command=0;
}

void IRTask::cleanup(){
    irrecv->disableIRIn();
    delete irrecv;
}