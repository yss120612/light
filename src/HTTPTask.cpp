#include "HTTPTask.h"
#include <Update.h>
//#include "Events.h"
#include <Wire.h>

void HTTPTask::cleanup(){
	server->end();
	SPIFFS.end();
	delete server;
}

void HTTPTask::setup(){
SPIFFS.begin();
server = new AsyncWebServer(80);
if (!server){
    Serial.println("Error creating HTTP server");
    return;
}
server->on("/", std::bind(&HTTPTask::handleRoot, this, std::placeholders::_1));
server->on("/upd", std::bind(&HTTPTask::handleUpd, this, std::placeholders::_1));
//server->on("/log", std::bind(&HTTPTask::handleUpd, this, std::placeholders::_1));
server->on("/reboot", std::bind(&HTTPTask::handleReboot, this, std::placeholders::_1));
server->on("/main", std::bind(&HTTPTask::handleMain, this, std::placeholders::_1));
server->on("/post", HTTP_ANY, std::bind(&HTTPTask::handleW2A, this, std::placeholders::_1));
server->on("/getdata", HTTP_ANY, std::bind(&HTTPTask::handleA2W, this, std::placeholders::_1));
server->onNotFound(std::bind(&HTTPTask::handleNotFound, this, std::placeholders::_1));
server->serveStatic("/css/bootstrap.min.css",SPIFFS,"/css/bootstrap.min.css");
server->serveStatic("/js/jquery.min.js",SPIFFS,"/js/jquery.min.js");
server->serveStatic("/js/bootstrap.min.js",SPIFFS,"/js/bootstrap.min.js");
server->serveStatic("/js/timepicker.min.js",SPIFFS,"/js/timepicker.min.js");
server->serveStatic("/css/font-awesome.min.css",SPIFFS,"/css/font-awesome.min.css");
server->serveStatic("/css/radio.css",SPIFFS,"/css/radio.css");
server->serveStatic("/fonts/fontawesome-webfont.woff2",SPIFFS,"/fonts/fontawesome-webfont.woff2");
	
server->on(
	"/update", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
	  request->redirect("/");
    }, 
	std::bind(&HTTPTask::handleUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
	);

server->on(
	"/spiffs", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
	    request->send(200);
    }, std::bind(&HTTPTask::handleSpiffs, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
	);



server->begin();

};


void HTTPTask::loop(){
   delay(10); 
   
}
    
void HTTPTask::handleMain(AsyncWebServerRequest * request) {
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/main.htm","text/html",request);
}

void HTTPTask::handleReboot(AsyncWebServerRequest * request) {
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/index.htm","text/html",request);
		ESP.restart();
}


void HTTPTask::handleRoot(AsyncWebServerRequest * request) {
	
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/index.htm","text/html", request);
}


void HTTPTask::handleLog(AsyncWebServerRequest * request) {
	
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/log.htm","text/html", request);
}

void HTTPTask::handleFile(String path,String type, AsyncWebServerRequest *request){
	request->send(SPIFFS,path,type);
}

void HTTPTask::handleNotFound(AsyncWebServerRequest * request) {
	request->send(200, "text/plain", "404 PAGE NOT FOUND!!!");
}


void HTTPTask::handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
 uint32_t free_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
 uint32_t result;
  if (!index){
	request->redirect("/");
    if (!Update.begin(free_space,U_FLASH)) {
    }
	else{
		result=800;
		xQueueSend(que,&result,portMAX_DELAY);
	}
  }

  if (Update.write(data, len) != len) {
  }else{
	  
  }

  if (final) {
    if (!Update.end(true)){
	  
    } else {
		ESP.restart();
    }
  }
}

void HTTPTask::handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
  uint32_t result;
  if (!index){
	request->redirect("/");
    if (!Update.begin(UPDATE_SIZE_UNKNOWN,U_SPIFFS)) {
    }
	else{
		result=800;
		xQueueSend(que,&result,portMAX_DELAY);
	}
  }
  if (Update.write(data, len) != len) {
  }else{
	//  if (counter++==9) {Serial.print(".");counter=0;}
  }
  if (final) {
    if (!Update.end(true)){
    } else {
	  ESP.restart();
    }
  }
}


void HTTPTask::handleUpd(AsyncWebServerRequest * request) {
    if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/upd.htm","text/html",request);
	
}

void HTTPTask::handleW2A(AsyncWebServerRequest * request)
{
	
	uint8_t params = request->params();
	if (params<1 || !(request->getParam(0)->name()).equals(F("page"))){
			request->send(500, F("text/plain"),F("ERROR PAGE PARAMETR")); // Oтправляем ответ No Reset
			return;
	}

    if (request->getParam(0)->value().equals(F("main"))){  
    	for (uint8_t i = 1; i < params; i++)
    	{
        	var(request->getParam(i)->name(), request->getParam(i)->value());
    	}
	} else if (request->getParam(0)->value().equals(F("log"))){  
    	for (uint8_t i = 1; i < params; i++)
    	{
        	//var_log(request->getParam(i)->name(), request->getParam(i)->value());
    	}
	}
    request->send(200, F("text/plain"), F("OK"));
}

void HTTPTask::var(String n, String v)
{
     event_t ev;
     ev.state=WEB_EVENT;
	uint8_t h=9,m=50,d=3,nn=0;
	if (n.equals("BTN1"))
	{
		ev.button=1;
		//Serial.println(v);
		h=v.substring(0,v.indexOf(':')).toInt();
		m=v.substring(v.indexOf(':')+1,v.indexOf('*')).toInt();
		d=v.substring(v.indexOf('*')+1,v.indexOf('-')).toInt();
		nn=v.substring(v.indexOf('-')+1).toInt();
		ev.alarm.hour=h;
		ev.alarm.minute=m;
		ev.alarm.period=(period_t)d;
		ev.alarm.action=nn;
	}
	else if (n.equals("BTN2"))
	{
		ev.button=2;
		//=makeAlarm(20,d,h,m); 
		//ev.count=v.equals(F("true"));
	}
	else if (n.equals("BTN3"))
	{
		ev.button=3;
		//=makeAlarm(20,d,h,m); 
		//ev.count=v.equals(F("true"));
	}
	else if (n.equals("BTN4"))
	{
		ev.button=4;
		//=makeAlarm(20,d,h,m); 
		//ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL1"))
	{
		ev.button=11;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL2"))
	{
		ev.button=12;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL3"))
	{
		ev.button=13;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL4"))
	{
		ev.button=14;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("FUNC1"))
	{
		ev.state=PULT_BUTTON;
		ev.button=PULT_STOP;
		ev.count=IR_DEVICE;
		
	}
	else if (n.equals("LIGHT_CW"))
	{
		ev.button=31;
		ev.count=v.toInt();
		
	}
	else if (n.equals("LIGHT_NW"))
	{
		ev.button=32;
		ev.count=v.toInt();
	}
	else if (n.equals("LIGHT_WW"))
	{
		ev.button=33;
		ev.count=v.toInt();
	}
	 xQueueSend(que,&ev,portMAX_DELAY);
}


String HTTPTask::getI2Cdevices(){
    int error;
	String res="I2C device found at address<ul>";
	
	
	uint8_t count=0;
    for (uint8_t address = 1; address < 127; address++ )  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)    {
		res+="<li>";
		res+=String(address,16);
		res+="</li>";
		count++;
      
    }
    }
    res+="<li><b>Ttal devices ";
	res+=String(count);
	res+="</b></li></ul>";
	return res;
}


void HTTPTask::handleA2W(AsyncWebServerRequest * request)
{
	if (request->params()<1 || !(request->getParam(0)->name()).equals("page")){
			request->send(500, F("text/plain"),F("ERROR PAGE PARAMETR")); // Oтправляем ответ No Reset
			return;
	}
	SystemState_t st;
	event_t ev;
	String str = F("{");
	if (request->getParam(0)->value().equals(F("log"))){
		//String str = F("{\"logdata\":\"<ul>")+logg.getAll2Web()+F("</ul>\"}");
		str+=F("\"logdata\":\"<ul>");
		//str+=logg.getAll2Web();
		str+=F("</ul>\"}");
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}else if (request->getParam(0)->value().equals(F("main"))){
		ev.button=199;
		ev.state=MEM_EVENT;
		xQueueSend(que,&ev,portMAX_DELAY);
		vTaskDelay(pdTICKS_TO_MS(100));
		//if (xMessageBufferReceive(web_mess,&st,SSTATE_LENGTH,portMAX_DELAY)==SSTATE_LENGTH){
		//if (xMessageBufferReceive(web_mess,&st,SSTATE_LENGTH,3000)==SSTATE_LENGTH){
		if (xMessageBufferReceive(web_mess,&st,SSTATE_LENGTH,100)==SSTATE_LENGTH)
		{
		for (uint8_t i=0;i<4;i++)
		{
			str+=F("\"REL");
			str+=String(i+1);
			str+=F("\":");
			str+=String(st.rel[i]?1:0);
			str+=F(",");
		}
		str+=F("\"BAND_CW\":\"");
		str+=String(st.br[0].value);
		str+=F("\",");
		str+=F("\"BAND_NW\":\"");
		str+=String(st.br[1].value);
		str+=F("\",");
		str+=F("\"BAND_WW\":\"");
		str+=String(st.br[2].value);
		str+=F("\"");
		for (uint8_t i=0;i<ALARMS_COUNT;i++)
		{
    		str+=(",\"ALRM");
			str+=String(i+1);
			str+=("\":\"");
			if (st.alr[i].active)
			{
			 str+=(st.alr[i].hour>9?String(st.alr[i].hour):"0"+String(st.alr[i].hour));
			 str+=("-");
			 str+=(st.alr[i].minute>9?String(st.alr[i].minute):"0"+String(st.alr[i].minute));
			 str+=(" Per=");
			 str+=String(st.alr[i].period);
			 str+=(" WD=");
			 str+=String(st.alr[i].wday);
			}
			else{
			 str+=("NONE");
			}
			str+=("\"");
		}
		str+=F("}");

		request->send(200, "text/json",str); // Oтправляем ответ No Reset
		}
	}else if (request->getParam(0)->value().equals(F("main1"))){
		str+=F("\"DEVSHOW\":\"");
		str+=getI2Cdevices();
		//str+="AAS";
		str+=F("\"}");
		//Serial.println(str);
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	
	}else if (request->getParam(0)->value().equals(F("update"))){
		
		str+=F("\"ALL\":");
		
		if (Update.isRunning()){
			str+=String(Update.size());
			str+=F(",\"PROGRESS\":");
			str+=String(Update.progress());
			str+=F(",\"WORK\":1,\"ERROR\":\"");
		}
		else
		{
			str+=F("0,\"PROGRESS\":0,\"WORK\":0,\"ERROR\":\"");
		}
		
		if (Update.hasError())
		{
			str+=Update.errorString();
		}
		else{
			str+=F("OK");
		}
		str+=F("}");

		//logg.logging(str);
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}
}

