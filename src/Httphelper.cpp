#include "Httphelper.h"
//#include <FS.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <SPIFFS.h>
#include "Settings.h"
#include "Log.h"

HttpHelper::HttpHelper()
{
	server = new AsyncWebServer(80);
}

HttpHelper::~HttpHelper()
{
	
	delete server;
	
}

void HttpHelper::setup(AppData *ad) {
	if (server == NULL) return;

	data = ad;

	server->on("/", std::bind(&HttpHelper::handleRoot, this, std::placeholders::_1));

	server->on("/test", std::bind(&HttpHelper::handleTest, this, std::placeholders::_1));

	server->on("/main", std::bind(&HttpHelper::handleMainFile, this, std::placeholders::_1));

    server->onNotFound(std::bind(&HttpHelper::handleNotFound, this, std::placeholders::_1));

	server->on("/upd", std::bind(&HttpHelper::handleUpd, this, std::placeholders::_1));

	server->on(
	"/update", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
	  request->redirect("/");
    }, 
	std::bind(&HttpHelper::handleUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
	);

server->on(
	"/spiffs", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
	    request->send(200);
    }, 
	std::bind(&HttpHelper::handleSpiffs, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6)
	);

	server->on("/log", std::bind(&HttpHelper::handleLog, this, std::placeholders::_1));

	server->on("/css/bootstrap.min.css", std::bind(&HttpHelper::handleBootstrapCss, this, std::placeholders::_1));
	server->on("/css/radio.css", std::bind(&HttpHelper::handleRadioCss, this, std::placeholders::_1));
	server->on("/css/font-awesome.min.css", std::bind(&HttpHelper::handleFontAwesomeCss, this, std::placeholders::_1));
	server->on("/fonts/fontawesome-webfont.woff2", std::bind(&HttpHelper::handleFontAwesomeFontsWoff, this, std::placeholders::_1));
	server->on("/css/progress.css", std::bind(&HttpHelper::handleProgressCss, this, std::placeholders::_1));
	server->on("/js/bootstrap.min.js", std::bind(&HttpHelper::handleBootstrapJs, this, std::placeholders::_1));
	server->on("/js/jquery.min.js", std::bind(&HttpHelper::handleJqueryJs, this, std::placeholders::_1));
	server->on("/js/progress.js", std::bind(&HttpHelper::handleProgressJs, this, std::placeholders::_1));


 	server->on("/post", HTTP_ANY, std::bind(&HttpHelper::handleW2A, this, std::placeholders::_1));

	server->on("/getdata", HTTP_ANY, std::bind(&HttpHelper::handleA2W, this, std::placeholders::_1));
		
	server->begin();


}

void HttpHelper::var(String n, String v)
{
	if (n.equals("REL1"))
	{
		data->putWebEvent(PULT_1,v.equals(F("true")));
	}
	else if (n.equals("REL2"))
	{
		data->putWebEvent(PULT_2,v.equals(F("true")));
	}
	else if (n.equals("REL3"))
	{
		data->putWebEvent(PULT_3,v.equals(F("true")));
	}
	else if (n.equals("REL4"))
	{
		data->putWebEvent(PULT_4,v.equals(F("true")));
	}
	else if (n.equals("BAND_CW"))
	{
		data->putWebEvent(WEB_CANNEL_CW,v.equals(F("true"))?255:0);
	}
	else if (n.equals("BAND_NW"))
	{
		data->putWebEvent(WEB_CANNEL_NW,v.equals(F("true"))?255:0);
	}
	else if (n.equals("BAND_WW"))
	{
		data->putWebEvent(WEB_CANNEL_WW,v.equals(F("true"))?255:0);
	}
}

void HttpHelper::var_log(String n, String v)
{
	if (n.equals("ACTION") && v.equals("clear"))
	{
		logg.clear();
		logg.logging("n=" + n + ", v=" + v);
	}
}



void HttpHelper::handleRoot(AsyncWebServerRequest * request) {
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/index.htm","text/html", request);
}



void HttpHelper::handleLog(AsyncWebServerRequest * request) {
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/log.htm","text/html",request);
}

void HttpHelper::handleNotFound(AsyncWebServerRequest * request) {
	request->send(200, "text/plain", "NOT FOUND!!!");
}


void HttpHelper::handleBootstrapCss(AsyncWebServerRequest * request) {
		handleFile("/css/bootstrap.min.css","text/css",request);
}

void HttpHelper::handleRadioCss(AsyncWebServerRequest * request) {
		handleFile("/css/radio.css","text/css",request);
}
void HttpHelper::handleFontAwesomeCss(AsyncWebServerRequest * request) {
		handleFile("/css/font-awesome.min.css","text/css",request);
}
void  HttpHelper::handleFontAwesomeFontsWoff(AsyncWebServerRequest * request){
	handleFile("/fonts/fontawesome-webfont.woff","font/woff",request);
}
void HttpHelper::handleProgressCss(AsyncWebServerRequest * request) {
		handleFile("/css/progress.css","text/css",request);
}

void HttpHelper::handleJqueryJs(AsyncWebServerRequest * request) {
    		handleFile("/js/jquery.min.js","application/javascript",request);
}

void HttpHelper::handleBootstrapJs(AsyncWebServerRequest * request) {
		handleFile("/js/bootstrap.min.js","application/javascript",request);
}

void HttpHelper::handleProgressJs(AsyncWebServerRequest * request) {
		handleFile("/js/progress.js","application/javascript",request);
}

void HttpHelper::handleFile(String path,String type, AsyncWebServerRequest *request){
	data->ir_sleep(); 
	//Serial.println(path);
	request->send(SPIFFS,path,type);
}

boolean HttpHelper::isUpdate(){
	return Update.isRunning();
}

void HttpHelper::handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
 uint32_t free_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
  if (!index){
	  
	this->data->ir_sleep(); 
	request->redirect("/");
	counter=0;
    //Serial.println("Update");
    if (!Update.begin(free_space,U_FLASH)) {
      	logg.logging("Update Error");
		logg.logging(Update.errorString());
    }
  }

  if (Update.write(data, len) != len) {
	logg.logging("Update Error");
	logg.logging(Update.errorString());
  }else{
	  if (counter++==9) {
		  counter=0;
		  }
  }

  if (final) {
    if (!Update.end(true)){
	  	logg.logging("Update Error");
	  	logg.logging(Update.errorString());
    } else {
		ESP.restart();
    }
  }
}

void HttpHelper::handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
 
  if (!index){
	this->data->ir_sleep(); 
	
	request->redirect("/");
	counter=0;
    logg.logging("Update SPIFFS");
    if (!Update.begin(UPDATE_SIZE_UNKNOWN,U_SPIFFS,LED)) {
      logg.logging(Update.errorString());
    }
  }

  if (Update.write(data, len) != len) {
    logg.logging(Update.errorString());
	
  }else{
	//  if (counter++==9) {Serial.print(".");counter=0;}
  }

  if (final) {
    if (!Update.end(true)){
      logg.logging(Update.errorString());
    } else {
      logg.logging("Update SPIFF complete. Rebooting...");
	  ESP.restart();
    }
  }
}


void HttpHelper::handleMainFile(AsyncWebServerRequest * request) {
	if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
		handleFile("/main.htm","text/html",request);
}

void HttpHelper::handleMainSetup(AsyncWebServerRequest * request)
{

}

void HttpHelper::handleW2A(AsyncWebServerRequest * request)
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
        	var_log(request->getParam(i)->name(), request->getParam(i)->value());
    	}
	}
    request->send(200, F("text/plain"), F("OK"));
}

void HttpHelper::handleA2W(AsyncWebServerRequest * request)
{
	if (request->params()<1 || !(request->getParam(0)->name()).equals("page")){
			request->send(500, F("text/plain"),F("ERROR PAGE PARAMETR")); // Oтправляем ответ No Reset
			return;
	}
	String str = F("{");
	if (request->getParam(0)->value().equals(F("log"))){
		//String str = F("{\"logdata\":\"<ul>")+logg.getAll2Web()+F("</ul>\"}");
		str+=F("\"logdata\":\"<ul>");
		str+=logg.getAll2Web();
		str+=F("</ul>\"}");
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}else if (request->getParam(0)->value().equals(F("main"))){
		for (uint8_t i=0;i<4;i++)
		{
			str+=F("\"REL");
			str+=String(i+1);
			str+=F("\":");
			str+=String(data->isOn(i)?1:0);
			str+=F(",");
		}
		str+=F("\"BAND_CW\":");
		str+=String(data->getCW());
		str+=F(",");
		str+=F("\"BAND_NW\":");
		str+=String(data->getNW());
		str+=F(",");
		str+=F("\"BAND_WW\":");
		str+=String(data->getWW());
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
		logg.logging(str);
		request->send(200, "text/json",str); // Oтправляем ответ No Reset
	}
}

String HttpHelper::text(String id, String label){
	String buf;
//     buf = F("{\"html\":\"input\",");
//     buf += String(F("\"id\":\"")) + id + "\",";
//     buf += F("\"type\":\"text\",");
//    // buf += String(F("\"value\":\"")) + param(id) + "\",";
//     buf += String(F("\"label\":\"")) + label + "\"";
//     buf += F("},");
	return buf;
}


String HttpHelper::checkbox(String id, String label){
    String buf = F("{\"html\":\"input\",");
//     buf += F("\"type\":\"checkbox\",");
//     buf += String(F("\"id\":\"")) + id + "\",";
//    // buf += String(F("\"value\":\"")) + param(id) + "\",";
//     buf += String(F("\"label\":\"")) + label + "\"";
//     buf += F("},");
	return buf;
}


void HttpHelper::handleUpd(AsyncWebServerRequest * request) {
    if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
	String resp = F("<!DOCTYPE html>\n<html>\n<head>\n");
	resp += F("<meta charset = \"utf-8\">\n");
	resp += F("<title>Yss desktop прошивка</title>\n");
	resp += F("<meta name = \"description\" content = \"Версия 0.1\">\n");
	resp += F("<meta name = \"author\" content = \"Yss\">\n");
	resp += F("<link href = \"/css/bootstrap.min.css\" type = \"text/css\" rel = \"stylesheet\">\n");
	resp += F("<link href = \"/css/progress.css\" type = \"text/css\" rel = \"stylesheet\">\n");
	resp += F("<script type = \"text/javascript\" src = \"/js/jquery.min.js\"></script>\n");
	resp += F("<script type = \"text/javascript\" src = \"/js/bootstrap.min.js\"></script>\n");
	resp += F("<script type = \"text/javascript\" src = \"/js/progress.js\"></script>\n");
	resp += F("</head>\n<body>\n");
	resp += F("<div class = \"col-md-12\">\n");
	resp += F("<a href = \"/\" class = \"btn btn-info\">Дом</a>\n");
	resp += F("</div>\n");
	resp += F("<div class = \"alert alert-info\" role = \"alert\">");
	resp += F("<h3>Прошивка</h3>\n");
	resp += F("<form method = \"POST\" action = \"/update\" enctype = \"multipart/form-data\" class=\"form-inline\">\n");
	resp += F("<div class = \"btn-group\">\n");
	resp += F("<input type = \"file\" class = \"btn\" name = \"update\">\n");
	resp += F("<input type = \"submit\" class = \"btn\" value = \"Прошить\" onclick = \"this.value = 'Подождите...';\">\n");
	resp += F("</div>\n");
	resp += F("</form>\n");
	resp += F("</div>\n");
	resp += F("<div class = \"alert alert-success\" role = \"alert\">");
	resp += F("<h3>WEB сервер</h3>\n");
	resp += F("<form method = \"POST\" action = \"/spiffs\" enctype = \"multipart/form-data\" class=\"form-inline\">");
	resp += F("<div class = \"btn-group\">\n");
	resp += F("<input type = \"file\" class = \"btn\" name = \"spiffs\">\n");
	resp += F("<input type = \"submit\" class = \"btn\" value = \"Прошить\" onclick = \"this.value = 'Подождите...';\">\n");
	resp += F("</div>\n");
	resp += F("</form>\n");

	resp += F("<div class = \"alert alert-warning\" role = \"alert\">");
	resp += F("<h3 class=\"progress-title\" id=\"PRGTITLE\"></h3>");
	resp += F("<div class=\"progress\">");
	resp += F("<div class=\"progress-bar\" style=\"width:0%; background:#97c513;\" id=\"PRG\">");
	resp += F("<div class=\"progress-value\" id=\"PRGVAL\">0%</div>");
	resp += F("</div>\n");
	resp += F("</div>\n");
	resp += F("</div>\n");
	resp += F("</div>\n");

	resp += F("</body>\n</html>\n");

	request->send(200, "text/html", resp);
}

void HttpHelper::handleTest(AsyncWebServerRequest * request) {
    if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
	String resp = "<!DOCTYPE html>\n<html>\n<head>\n";
	resp += "<meta charset = \"utf-8\">\n";
	resp += "<title>YssLight Test</title>\n";
	resp += "<meta name = \"description\" content = \"Версия 0.1\">\n";
	resp += "<meta name = \"author\" content = \"Yss\">\n";
	resp += "<link href = \"/css/bootstrapcss\" type = \"text/css\" rel = \"stylesheet\">\n";
	resp += "<script type = \"text/javascript\" src = \"/js/bootstrapjs\"></script>\n";
	resp += "</head>\n<body>\n";
	resp += "<div>\n";
	resp += "<a href = \"/\">Дом</a>\n";
	resp += "</div>\n";
	resp += "<div>";
	resp += "<h3>Тест</h3>\n";
	resp += "</div>\n";
	resp += "</body>\n</html>\n";

	request->send(200, "text/html", resp);
}

