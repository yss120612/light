#include "MEMTask.h"
//#include "Events.h"

void MEMTask::setup()
{
	Wire.begin();
	read_state();
}

void MEMTask::cleanup()
{
}

void MEMTask::reset_memory(){
uint8_t i;
		for(i=0;i<RELAYS_COUNT;i++) sstate.rel[i] = false;
		for(i=0;i<LEDS_COUNT;i++) {sstate.br[i].value = 0;sstate.br[i].stste=BLINK_ON;}
		for (i = 0; i < ALARMS_COUNT; i++)
		{
			sstate.alr[i].action = 0;
			sstate.alr[i].period = ONCE_ALARM;
			sstate.alr[i].active = false;
			sstate.alr[i].hour = 0;
			sstate.alr[i].minute = 0;
			sstate.alr[i].wday = 0;
		}
		sstate.version = VER;
		write_state();
}

void MEMTask::read_state()
{
	uint8_t counter=0;
	uint8_t crc=1;
	while (counter<2 && crc!=0){
	try{
	read(0, (uint8_t *)&sstate, sizeof(sstate));
	}
	catch (const char * error_message){
	#ifdef DEBUGG	
	Serial.println(error_message);
	#endif	
	}
	crc=crc8((uint8_t *)&sstate,sizeof(sstate));
	counter++;
	}
	
	#ifdef DEBUGG
	if (crc!=0)	Serial.println("BAD CRC!!!");
	#endif

	if (VER!=sstate.version || crc!=0)
	{
		reset_memory();
	}
}

void MEMTask::write_state()
{
	sstate.crc=crc8((uint8_t *)&sstate, sizeof(sstate)-1);
	write(0, (uint8_t *)&sstate, sizeof(sstate));
}

void MEMTask::loop()
{
	uint32_t command;
	event_t ev;
	notify_t nt;
	if (xTaskNotifyWait(0, 0, &command, portMAX_DELAY))
	//if (xTaskNotifyWait(0, 0, &command, pdMS_TO_TICKS(1000)))
	{
		
		memcpy(&nt,&command,sizeof(command));
		
		switch (nt.title)
		{
		
	
		case 10:	
		case 11:
		case 12:
		//case 13:
		sstate.rel[nt.title-10]=nt.packet.value;
		#ifdef DEBUGG
		Serial.print("Save relay");
        Serial.print(nt.title-10);
        Serial.print("=");
        Serial.println(nt.packet.value);
		#endif
		write_state();
		//xTaskNotifyStateClear(NULL);
		
		break;
		case 20:	
		case 21:
		case 22:
		sstate.br[nt.title-20].stste=(blinkmode_t)nt.packet.var;
		sstate.br[nt.title-20].value=nt.packet.value;
		#ifdef DEBUGG
		Serial.print("Save leds band=");
        Serial.print(nt.title-20);
        Serial.print(" brightness is ");
        Serial.print(nt.packet.value);
		Serial.print(" mode is ");
        Serial.println(nt.packet.var);
		#endif
		try{
		write_state();
		//Serial.println("Saved");
		}catch (const char * error_message){
			#ifdef DEBUGG
			Serial.println(error_message);
			#endif
		}
		//xTaskNotifyStateClear(NULL);
		
		break;
		case 100:	
		case 101:	
		case 102:	
		case 103:	
		case 104:	
		case 105:	
		case 106:	
		case 107:	
		case 108:	
		case 109:	
		#ifdef DEBUGG
		Serial.print("Save alarm=");
        Serial.print(nt.title-100);
        Serial.print(" per=");
        Serial.print(nt.alarm.period);
		Serial.print(" ");
		Serial.print(nt.alarm.hour);
		Serial.print(":");
		Serial.print(nt.alarm.minute);
		Serial.print(" ACTIVE:");
		Serial.println(nt.alarm.active?"YES":"NO");
        #endif
		sstate.alr[nt.title-100]=nt.alarm;
		try{
		write_state();
		//Serial.println("Saved");
		}catch (const char * error_message){
			#ifdef DEBUGG
			Serial.println(error_message);
			#endif
		}
		break;
		case 199:
	   		xMessageBufferSend(web_mess, &sstate, SSTATE_LENGTH, portMAX_DELAY);
			break;
		case 200:
	   		xMessageBufferSend(alarm_mess, &sstate, SSTATE_LENGTH, portMAX_DELAY);
			break;
		case 201://reset alarms
		for (uint8_t i = 0; i < ALARMS_COUNT; i++)
			{
			sstate.alr[i].action = 0;
			sstate.alr[i].period = ONCE_ALARM;
			sstate.alr[i].active = false;
			sstate.alr[i].hour = 0;
			sstate.alr[i].minute = 0;
			sstate.alr[i].wday = 0;
			}
			write_state();
	   		xMessageBufferSend(alarm_mess, &sstate, SSTATE_LENGTH, portMAX_DELAY);
			break;	
		case 202://reset all
	   		reset_memory();
			xMessageBufferSend(alarm_mess, &sstate, SSTATE_LENGTH, portMAX_DELAY);
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
			 //Serial.printf("%d=%d; ",idx,*buf);
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
			// Serial.printf("%d=%d; ",idx,*buf);
			*buf ++;
	}
	//Serial.println(".");
	Wire.endTransmission();
	vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIMEOUT));
}