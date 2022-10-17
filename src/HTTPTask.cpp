#include "HTTPTask.h"
#include <Update.h>
#include "Events.h"

void HTTPTask::cleanup(){
	server->end();
	SPIFFS.end();
	delete server;
};
void HTTPTask::setup(){
SPIFFS.begin();
server = new AsyncWebServer(80);
if (!server){
    Serial.println("Error creating HTTP server");
    return;
}
server->on("/", std::bind(&HTTPTask::handleRoot, this, std::placeholders::_1));
server->on("/upd", std::bind(&HTTPTask::handleUpd, this, std::placeholders::_1));
server->on("/log", std::bind(&HTTPTask::handleUpd, this, std::placeholders::_1));
server->on("/main", std::bind(&HTTPTask::handleMain, this, std::placeholders::_1));
server->on("/post", HTTP_ANY, std::bind(&HTTPTask::handleW2A, this, std::placeholders::_1));
server->on("/getdata", HTTP_ANY, std::bind(&HTTPTask::handleA2W, this, std::placeholders::_1));
server->onNotFound(std::bind(&HTTPTask::handleNotFound, this, std::placeholders::_1));
server->serveStatic("/css/bootstrap.min.css",SPIFFS,"/css/bootstrap.min.css");
server->serveStatic("/js/jquery.min.js",SPIFFS,"/js/jquery.min.js");
server->serveStatic("/js/bootstrap.min.js",SPIFFS,"/js/bootstrap.min.js");
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
  	if (n.equals("REL1"))
	{
		ev.button=PULT_1;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL2"))
	{
		ev.button=PULT_2;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL3"))
	{
		ev.button=PULT_3;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("REL4"))
	{
		ev.button=PULT_4;
		ev.count=v.equals(F("true"));
	}
	else if (n.equals("BAND_CW"))
	{
		ev.button=WEB_CANNEL_CW;
		ev.count=v.equals(F("true"))?255:0;
		
	}
	else if (n.equals("BAND_NW"))
	{
		ev.button=WEB_CANNEL_NW;
		ev.count=v.equals(F("true"))?255:0;
	}
	else if (n.equals("BAND_WW"))
	{
		ev.button=WEB_CANNEL_WW;
		ev.count=v.equals(F("true"))?255:0;
	}
	xQueueSend(que,&ev,portMAX_DELAY);
}


void HTTPTask::handleA2W(AsyncWebServerRequest * request)
{
	if (request->params()<1 || !(request->getParam(0)->name()).equals("page")){
			request->send(500, F("text/plain"),F("ERROR PAGE PARAMETR")); // Oтправляем ответ No Reset
			return;
	}
	String str = F("{");
	if (request->getParam(0)->value().equals(F("log"))){
		//String str = F("{\"logdata\":\"<ul>")+logg.getAll2Web()+F("</ul>\"}");
		str+=F("\"logdata\":\"<ul>");
		//str+=logg.getAll2Web();
		str+=F("</ul>\"}");
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}else if (request->getParam(0)->value().equals(F("main"))){
		for (uint8_t i=0;i<4;i++)
		{
			str+=F("\"REL");
			str+=String(i+1);
			str+=F("\":");
			//str+=String(data->isOn(i)?1:0);
			str+=F(",");
		}
		str+=F("\"BAND_CW\":");
		str+=String(128);
		str+=F(",");
		str+=F("\"BAND_NW\":");
		str+=String(128);
		str+=F(",");
		str+=F("\"BAND_WW\":");
		str+=String(128);
		str+=F("}");

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

