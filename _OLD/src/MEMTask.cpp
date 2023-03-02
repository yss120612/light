#include "MEMTask.h"


void MEMTask::setup(){
//_address = AT24C32_ADDRESS;
 Wire.begin();  
}


void MEMTask::cleanup(){

}




void MEMTask::loop(){
uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    uint8_t comm,value;
    uint16_t addr;
	readPacket(command,&comm,&value,&addr);
	//comm=1 read, com=2 write
		   Serial.print(comm==1?"read":"write");
		   Serial.print(" addr=");
		   Serial.print(addr);
		   Serial.print(" value=");
		   Serial.println(value);
    switch (comm)
    {

      case 1:
           	read(addr,&value,sizeof(value)); 
          	event_t ev;
           	ev.state=MEM_EVENT;
           	ev.button=addr;
           	ev.count=value;
        	xQueueSend(que,&ev,portMAX_DELAY);
      break;
      case 2:
           write(addr,&value,sizeof(value)); 
      break;
    }
  }
  
}

void MEMTask::read(uint16_t index, uint8_t *buf, uint16_t len)
{
	index += AT24C32_OFFSET;
	index &= 0x0FFF;
	uint16_t idx=0;
	uint8_t count=0;
	//Serial.println("Read ");
	for (idx=0;idx<len;idx++){
		if (((index+idx)%EEPROM_PAGE_SIZE==0) || idx==0){
			 Wire.beginTransmission(AT24C32_ADDRESS);
			 vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
    		 Wire.write(((index+idx) >> 8) & 0x0F);
			 Wire.write((index+idx) & 0xFF);
    		 Wire.endTransmission();
			 vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
			 if (idx==0){
				count=(index % EEPROM_PAGE_SIZE)==0?len>=EEPROM_PAGE_SIZE?EEPROM_PAGE_SIZE:len:EEPROM_PAGE_SIZE-(index % EEPROM_PAGE_SIZE);
			 }
			 else{
				count=len-idx>=EEPROM_PAGE_SIZE?EEPROM_PAGE_SIZE:len-idx;
			 }
			 Wire.requestFrom(AT24C32_ADDRESS, count);	
			 vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
			}
			
			*buf = Wire.read();
	//		 Serial.printf("%d=%d; ",idx,*buf);
			*buf ++;
			

	}
	//Serial.println(".");
}



void MEMTask::write(uint16_t index, const uint8_t *buf, uint16_t len)
{
	index += AT24C32_OFFSET;
	index &= 0x0FFF;
	uint16_t idx=0;
	//Serial.print("Write ");
	for (idx=0;idx<len;idx++){
		if (((index+idx)%EEPROM_PAGE_SIZE==0) || idx==0){
			if (idx>0) {
				Wire.endTransmission();
				vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
				}
    		Wire.beginTransmission(AT24C32_ADDRESS);
			vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
    		Wire.write(((index+idx) >> 8) & 0x0F);
			Wire.write((index+idx) & 0xFF);
			vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
			}
			Wire.write(*buf);
			 //Serial.printf("%d=%d; ",idx,*buf);
			*buf ++;
	}
	//Serial.println(".");
	Wire.endTransmission();
	vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
}