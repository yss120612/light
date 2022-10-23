#include "MEMTask.h"
#include "Events.h"

void MEMTask::setup(){
_address = AT24C32_ADDRESS;
 Wire.begin();  
}


void MEMTask::cleanup(){

}




void MEMTask::loop(){
uint32_t command;
  
  if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
  {
    uint8_t comm,value;//=command>>24 & 0x000000FF;
    uint16_t addr;
	readPacket(command,comm,value,addr);
    //comm=1 read, com=2 write
    switch (comm)
    {

      case 1:
	  break;
           read(addr,&value,sizeof(value)); 
           event_t ev;
           ev.state=MEM_EVENT;
           ev.button=addr;
           ev.count=value;
           
        Serial.print("Addr="); 
        Serial.print(ev.button);
        Serial.print(" Value="); 
        Serial.println(ev.count);
        xQueueSend(que,&ev,portMAX_DELAY);
      break;
      case 2:
	  break;
	  	   Serial.print("addr=");
		   Serial.print(addr);
		   Serial.print("value=");
		   Serial.println(value);
           write(addr,&value,sizeof(value)); 
      break;
    }
  }
  
}

void MEMTask::read(uint16_t index, uint8_t* buf, uint16_t len) {
	
	Wire.beginTransmission(_address);
	Wire.write((index >> 8) & 0x0F);
	Wire.write(index & 0xFF);
	if (Wire.endTransmission() == 0) {
		while (len > 0) {
			uint8_t l;

			l = EEPROM_WORK_SIZE;
			if (l > len)
				l = len;
			len -= l;
			Wire.requestFrom(_address, l);
			delay(10);
			while (l--)
				*buf++ = Wire.read();
		}
	}
}


void MEMTask::write(uint16_t index, const uint8_t* buf, uint16_t len) {
	
	index &= 0x0FFF;
	while (len > 0) {
		uint8_t l;

		l = EEPROM_WORK_SIZE - (index % EEPROM_WORK_SIZE);
		if (l > len)
			l = len;
		len -= l;
		Wire.beginTransmission(_address);
		Wire.write(index >> 8);
		Wire.write(index & 0xFF);
		while (l--) {
			Wire.write(*buf++);
			++index;
		}
		delay(10);
		if (Wire.endTransmission() != 0)
			break;
		while (!Wire.requestFrom(_address, (uint8_t)1)); // Polling EEPROM ready (write complete)
	}
}