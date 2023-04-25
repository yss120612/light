#include "IRTask.h"

void IRTask::setup(){
irrecv = new IRrecv(IRPIN);
irrecv->enableIRIn();
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
        #ifdef DEBUGG
        // Serial.print("IR Command="); 
        // Serial.print(ev.button);
        // Serial.print(" Address="); 
        // Serial.print(ev.count);
        // Serial.print(" Type="); 
        // Serial.println(ev.data);
        #endif
        //if (dres.address==IR_DEVICE){
        xQueueSend(que,&ev,portMAX_DELAY);
        //}
        
        }
        irrecv->resume();
    }
        
    vTaskDelay(pdMS_TO_TICKS(500));
    old_command=0;

}

void IRTask::cleanup(){
    irrecv->disableIRIn();
    delete irrecv;
}