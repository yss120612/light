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
	//SPIFFS.begin();
	
}

HttpHelper::~HttpHelper()
{
	//delete httpUpdater;
	//delete httpSpiffsUpdater;
	delete server;
	//SPIFFS.end();
}

void HttpHelper::setup(IRreceiver * rcv) {
	if (server == NULL) return;

	irrc=rcv;
	//WiFiconnect();
			
	//server->on("/", std::bind(&HttpHelper::handleRoot, this, std::placeholders::_1));

	//server->serveStatic("/", SPIFFS, "/index.htm", NULL);
	//server->serveStatic("", SPIFFS, "/index.htm", NULL);
	server->on("/", std::bind(&HttpHelper::handleRoot, this, std::placeholders::_1));

	server->on("/test", std::bind(&HttpHelper::handleTest, this, std::placeholders::_1));

	server->on("/main", std::bind(&HttpHelper::handleMainFile, this, std::placeholders::_1));

	server->on("/logdata", std::bind(&HttpHelper::handleLogData, this, std::placeholders::_1));

    server->onNotFound(std::bind(&HttpHelper::handleNotFound, this, std::placeholders::_1));

	// server->on("/distilldata", std::bind(&HttpHelper::handleDistill, this));
	// server->on("/distillset", std::bind(&HttpHelper::handleDistillSet, this)); // Установка distill

	// server->on("/rectifydata", std::bind(&HttpHelper::handleRectify, this));
	// server->on("/rectifyset", std::bind(&HttpHelper::handleRectifySet, this)); // Установка rectify

	// //server->on("/suviddata", std::bind(&HttpHelper::handleSuvid, this));
	// //server->on("/suvidset", std::bind(&HttpHelper::handleSuvidSet, this)); // Установка suvid

	// server->on("/brewingdata", std::bind(&HttpHelper::handleBrewing, this));
	// server->on("/brewingset", std::bind(&HttpHelper::handleBrewingSet, this));

	//server->on("/restart", web_handlers::restart);
		
	server->on("/upd", std::bind(&HttpHelper::handleUpd, this, std::placeholders::_1));

	server->on(
	"/update", 
	HTTP_POST, 
	[](AsyncWebServerRequest *request){
      //request->send(200);
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
	server->on("/js/bootstrap.min.js", std::bind(&HttpHelper::handleBootstrapJs, this, std::placeholders::_1));
	server->on("/js/jquery.min.js", std::bind(&HttpHelper::handleJqueryJs, this, std::placeholders::_1));

	//server->serveStatic("/heater",SPIFFS,"/heater.htm", NULL);

	//server->serveStatic("/log", SPIFFS, "/log.htm", NULL);

	//server->serveStatic("/distill", SPIFFS, "/distillation.htm", NULL);

	//server->serveStatic("/rectify", SPIFFS, "/rectify.htm", NULL);

	//server->serveStatic("/suvid", SPIFFS, "/suvid.htm", NULL);

	//server->serveStatic("/brewing", SPIFFS, "/brewing.htm", NULL);

	//server->serveStatic("/css/bootstrap.min.css", SPIFFS, "/css/bootstrap.min.css", NULL);

	//server->serveStatic("/js/bootstrap.min.js", SPIFFS, "/js/bootstrap.min.js", NULL);

	//server->serveStatic("/js/jquery.min.js", SPIFFS, "/js/jquery.min.js", NULL);

	//server->serveStatic("/js/export-data.js", SPIFFS, "/js/export-data.js", NULL);

	//server->serveStatic("/js/exporting.js", SPIFFS, "/js/exporting.js", NULL);
 
	//server->serveStatic("/js/highstock.js", SPIFFS, "/js/highstock.js", NULL);

	in_update=false;		
	server->begin();

//	httpSpiffsUpdater = new 
//	httpSpiffsUpdater->setup(server);
}

boolean HttpHelper::isConnected()
{
	//return WiFi.status() == WL_CONNECTED;
	return true;
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


void HttpHelper::handleLogData(AsyncWebServerRequest * request)
{
	String str = "{\"logdata\":\"<ul>"+logg.getAll2Web()+"</ul>\"}";
	request->send(200, "text/json",str); // Oтправляем ответ No Reset
}


void HttpHelper::handleJqueryJs(AsyncWebServerRequest * request) {
    		handleFile("/js/jquery.min.js","application/javascript",request);
}

void HttpHelper::handleBootstrapCss(AsyncWebServerRequest * request) {
		handleFile("/css/bootstrap.min.css","text/css",request);
}

void HttpHelper::handleBootstrapJs(AsyncWebServerRequest * request) {
		handleFile("/js/bootstrap.min.js","application/javascript",request);
}

void HttpHelper::handleFile(String path,String type, AsyncWebServerRequest *request){
	irrc->sleep_sometime(); 
	//Serial.println(path);
	request->send(SPIFFS,path,type);
}

void HttpHelper::handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
 uint32_t free_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
  if (!index){
	irrc->sleep_sometime();
	in_update=true;
	request->redirect("/"); 
	counter=0;
    Serial.println("Update");
    if (!Update.begin(free_space,U_FLASH)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
	//irrc->enable();
	in_update=false;
  }else{
	  if (counter++==9) {Serial.print(".");counter=0;}
  }

  if (final) {
    if (!Update.end(true)){
	  		
      Update.printError(Serial);
    } else {
	  Serial.println("");
      Serial.println("Update complete. Rebooting...");
	//  request->send()
	  ESP.restart();
    }
  }
}

void HttpHelper::handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
 
  if (!index){
	irrc->sleep_sometime();
	in_update=true;
	request->redirect("/");
	counter=0;
    Serial.println("Update SPIFFS");
    if (!Update.begin(UPDATE_SIZE_UNKNOWN,U_SPIFFS,LED)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
	in_update=false;
  }else{
	  if (counter++==9) {Serial.print(".");counter=0;}
  }

  if (final) {
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
	  Serial.println("");
      Serial.println("Update SPIFF complete. Rebooting...");
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
	String str = "{\"logdata\":\"<ul>"+logg.getAll2Web()+"</ul>\"}";
	request->send(200, "text/json",str); // Oтправляем ответ No Reset
}

void HttpHelper::handleMainW2A(AsyncWebServerRequest * request)
{
	String str = "{\"logdata\":\"<ul>"+logg.getAll2Web()+"</ul>\"}";
	request->send(200, "text/json",str); // Oтправляем ответ No Reset
}

void HttpHelper::handleMainA2W(AsyncWebServerRequest * request)
{
	String str = "{\"logdata\":\"<ul>"+logg.getAll2Web()+"</ul>\"}";
	request->send(200, "text/json",str); // Oтправляем ответ No Reset
}


String HttpHelper::text(String id, String label){
	String buf;
    buf = F("{\"html\":\"input\",");
    buf += String(F("\"id\":\"")) + id + "\",";
    buf += F("\"type\":\"text\",");
   // buf += String(F("\"value\":\"")) + param(id) + "\",";
    buf += String(F("\"label\":\"")) + label + "\"";
    buf += F("},");
	return buf;
}


String HttpHelper::checkbox(String id, String label){
    String buf = F("{\"html\":\"input\",");
    buf += F("\"type\":\"checkbox\",");
    buf += String(F("\"id\":\"")) + id + "\",";
   // buf += String(F("\"value\":\"")) + param(id) + "\",";
    buf += String(F("\"label\":\"")) + label + "\"";
    buf += F("},");
	return buf;
}

// void HttpHelper::handleDistill()
// {
// 	String str =  "AAA";
// 	server->send(200, "text/json", str); // Oтправляем ответ No Reset
// }

// void HttpHelper::handleDistillSet()
// {
// 	//ds->setData(SET_DISTILLTSTOP, server->arg("TSTOP"));
// 	//logg.logging("Temperature (cube) end of distill set on " + server->arg("TSTOP"));
// 	//server->send(200, "text/plain", "OK");
// }

// void HttpHelper::handleRectify()
// {
	
// 	String str =   "BBB";
// 	server->send(200, "text/json", str); // Oтправляем ответ No Reset
// }

// void HttpHelper::handleRectifySet()
// {
// 	//ds->setData(SET_RECTTSTOP, server->arg("TSTOPSET"));
// 	//logg.logging("Temperature (def) end of rectify sett on " + server->arg("TSTOPSET"));
// 	//server->send(200, "text/plain", "OK");
// }

//void HttpHelper::handleSuvid(){
//	String str = "{\"kube_data\":" + ds->getData(DS_TKUBE) 
//		      + ", \"cooler_data\":" + ds->getData(DS_TTRIAK) 
//		      + ", \"heater_data\":" + ds->getData(DS_HPOWER) 
//		      + ", \"ttarget_data\":\"" + ds->getData(DS_SUVIDTARGET)
//			  + "\", \"timetarget_data\":\"" + ds->getData(DS_SUVIDTIME)
//		      + "\", \"state_data\":\"" + ds->getData(DS_SUVIDSTATE) 
//		      + "\", \"time_data\":\"" + ds->getData(DS_SUVIDTIMELEFT) + "\" }";
//	server->send(200, "text/json", str); // Oтправляем ответ No Reset
//}
//
//void HttpHelper::handleSuvidSet()
//{
//	ds->setData(SET_SUVIDTEMP, server->arg("TMP"));
//	logg.logging("SUVID Temperature set on " + server->arg("TMP"));
//	ds->setData(SET_SUVIDTIME, server->arg("TIME"));
//	logg.logging("SUVID Time set on " + server->arg("TIME"));
//}

// void HttpHelper::handleBrewing() {
// 	String str = "WWW";
// 	server->send(200, "text/json", str); // Oтправляем ответ No Reset
// }

// void HttpHelper::handleBrewingSet()
// {
// 	uint8_t part = (server->arg("PART")).toInt();
// 	switch (part) {
// 	case 1:
// 	case 2:
// 	case 3:
// 	case 4:
// 		//ds->setData(SET_BREWINGTMP1+(part-1)*2, server->arg("TMP"));
// 		//logg.logging("BREWING Temperature " + String(part) +" set on " + server->arg("TMP"));
// 		//ds->setData(SET_BREWINGTIME1 + (part - 1) * 2, server->arg("TIME"));
// 		//logg.logging("BREWING Time " + String(part) + " set on " + server->arg("TIME"));
// 		break;
// 	case 5:
// 		//ds->setData(SET_BREWINGCHILLER, server->arg("CHILLER"));
// 		break;

// 	}
// }

// void HttpHelper::WiFiconnect()
// {
	// WiFi.disconnect();
	// //IPAddress apIP(192, 168, 0, 100);
	// WiFi.mode(WIFI_STA);
	// //WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	// //WiFi.softAP(conf->getWiFiN().c_str(), conf->getWiFiP().c_str());
	// uint8_t modeWiFi = 0;
	// int n_network = WiFi.scanNetworks(); // запрос количества доступных сетей
	// for (int i = 0; i < n_network; ++i) {
	// 	//logg.logging("network " + WiFi.SSID(i) + " present");
	// 	//if (WiFi.SSID(i).equals(CONF.getWiFiN()))
	// 	//{
	// //		modeWiFi = 1; // наша сеть присутствует
	// //		break;
	// //	}

	// }

	// if (modeWiFi == 1) {
	// 	// пробуем подключиться

	// 	//logg.logging("Connecting to " + CONF.getWiFiN());

	// 	WiFi.disconnect(true);
	// 	WiFi.begin(CONF.getWiFiN().c_str(), CONF.getWiFiP().c_str());
	// 	// ждем N кол-во попыток, если нет, то AP Mode
	// 	byte tmp_while = 0;
	// 	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
	// 		delay(1000);

	// 		logg.logging("Connecting to " + CONF.getWiFiN());

	// 		if (tmp_while < 5) tmp_while++;
	// 		else {
	// 			modeWiFi = 0;
	// 			logg.logging("Connection to " + CONF.getWiFiN() + " failed!");

	// 			break;
	// 		}
	// 	}
	// }
// }

// void HttpHelper::WiFiReconnect()
// {
	// if (isConnected()) {
	// 	return;
	// }

	// WiFi.reconnect();
	// //logg.logging("Reconnect...");

	// // При потери связи с базовой станцией переходим в режим точки доступа и пробуем переподключиться
	// /*if (conf->getWiFiN().length() && tCnt >= setRestartWiFi && !WiFi.softAPgetStationNum()) {
	// WiFi.reconnect();
	// Serial.println("reconnect");
	// }*/
// }

void HttpHelper::handleUpd(AsyncWebServerRequest * request) {
    if (!request->authenticate("Yss1", "bqt3"))
		return request->requestAuthentication();
	String resp = "<!DOCTYPE html>\n<html>\n<head>\n";
	resp += "<meta charset = \"utf-8\">\n";
	resp += "<title>YssSM прошивка</title>\n";
	resp += "<meta name = \"description\" content = \"Версия 0.1\">\n";
	resp += "<meta name = \"author\" content = \"Yss\">\n";
	resp += "<link href = \"/css/bootstrap.min.css\" type = \"text/css\" rel = \"stylesheet\">\n";
	resp += "<script type = \"text/javascript\" src = \"/js/jquery.min.js\"></script>\n";
	resp += "<script type = \"text/javascript\" src = \"/js/bootstrap.min.js\"></script>\n";
	resp += "</head>\n<body>\n";
	resp += "<div class = \"col-md-12\">\n";
	resp += "<a href = \"/\"class = \"btn btn-info\">Дом</a>\n";
	resp += "</div>\n";
	resp += "<div class = \"alert alert-info\" role = \"alert\">";
	resp += "<h3>Прошивка</h3>\n";
	resp += "<form method = \"POST\" action = \"/update\" enctype = \"multipart/form-data\" class=\"form-inline\">\n";
	resp += "<div class = \"btn-group\">\n";
	resp += "<input type = \"file\" class = \"btn\" name = \"update\">\n";
	resp += "<input type = \"submit\" class = \"btn\" value = \"Прошить\" onclick = \"this.value = 'Подождите...';\">\n";
	resp += "</div>\n";
	resp += "</form>\n";
	resp += "</div>\n";
	resp += "<div class = \"alert alert-success\" role = \"alert\">";
	resp += "<h3>WEB сервер</h3>\n";
	resp += "<form method = \"POST\" action = \"/spiffs\" enctype = \"multipart/form-data\" class=\"form-inline\">";
	resp += "<div class = \"btn-group\">\n";
	resp += "<input type = \"file\" class = \"btn\" name = \"spiffs\">\n";
	resp += "<input type = \"submit\" class = \"btn\" value = \"Прошить\" onclick = \"this.value = 'Подождите...';\">\n";
	resp += "</div>\n";
	resp += "</form>\n";
	resp += "</div>\n";
	resp += "</body>\n</html>\n";

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



// void HttpHelper::clientHandle() {
	//if (server!=NULL) server->handleClient();
	//ArduinoOTA.handle();
// }

// boolean HttpHelper::handleFileRead(String path) {

// 	if (path.endsWith("/")) path += "index.htm";
// 	String contentType;
// 	if (path.endsWith(".htm") || path.endsWith(".html")) contentType = "text/html";
// 	else if (path.endsWith(".css")) contentType = "text/css";
// 	else if (path.endsWith(".js")) contentType = "application/javascript";
// 	else if (path.endsWith(".png")) contentType = "image/png";
// 	else if (path.endsWith(".gif")) contentType = "image/gif";
// 	else if (path.endsWith(".jpg")) contentType = "image/jpeg";
// 	else if (path.endsWith(".ico")) contentType = "image/x-icon";
// 	else if (path.endsWith(".xml")) contentType = "text/xml";
// 	else if (path.endsWith(".pdf")) contentType = "application/x-pdf";
// 	else if (path.endsWith(".zip")) contentType = "application/x-zip";
// 	else if (path.endsWith(".gz")) contentType = "application/x-gzip";
// 	else if (path.endsWith(".json")) contentType = "application/json";
// 	else contentType = "text/plain";

// 	//split filepath and extension

// 	String prefix = path, ext = "";
// 	int lastPeriod = path.lastIndexOf('.');
// 	if (lastPeriod >= 0) {
// 		prefix = path.substring(0, lastPeriod);
// 		ext = path.substring(lastPeriod);
// 	}

// 	//look for smaller versions of file
// 	//minified file, good (myscript.min.js)
// 	if (SPIFFS.exists(prefix + ".min" + ext)) path = prefix + ".min" + ext;
// 	//gzipped file, better (myscript.js.gz)
// 	if (SPIFFS.exists(prefix + ext + ".gz")) path = prefix + ext + ".gz";
// 	//min and gzipped file, best (myscript.min.js.gz)
// 	if (SPIFFS.exists(prefix + ".min" + ext + ".gz")) path = prefix + ".min" + ext + ".gz";

// 	if (SPIFFS.exists(path)) {

// 		File file = SPIFFS.open(path, "r");
// 		//if (server->hasArg())
// 		if (server->hasArg("download"))
// 			server->sendHeader("Content-Disposition", " attachment;");
// 		if (server->uri().indexOf("nocache") < 0)
// 			server->sendHeader("Cache-Control", " max-age=172800");

// 		//optional alt arg (encoded url), server sends redirect to file on the web
// 		//if (WiFi.status() == WL_CONNECTED && ESP8266WebServer::hasArg("alt")) {
// 		//	ESP8266WebServer::sendHeader("Location", ESP8266WebServer::arg("alt"), true);
// 		//	ESP8266WebServer::send(302, "text/plain", "");
// 		//}
// 		//else {
// 		//	//server sends file
// 		//	size_t sent = ESP8266WebServer::streamFile(file, contentType);
// 		//}
// 		size_t sent = server->streamFile(file, contentType);
// 		file.close();
// 		return true;
// 	} //if SPIFFS.exists

// 	//server->send(200, "text/plain", SPIFFS.);
// 	return false;
// } //bool handleFileRead

