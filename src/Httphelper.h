
#ifndef _HTTPHELPER_h
#define _HTTPHELPER_h

//#include <WiFi.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "IRreceiver.h"
#include "Data.h"

class HttpHelper
{
public:
	HttpHelper();
	~HttpHelper();

	// void clientHandle();
	//boolean handleFileRead(String path);
	void setup(IRreceiver * rcv, AppData * ad);
	//void setDataSource(DataSrc * d) { ds = d; };
	
	
	/*void setMode(Mode * m);
	Mode * getMode();*/
boolean isUpdate(){return in_update;}
private:
	void handleFile(String path,String type, AsyncWebServerRequest *request);
	void handleRoot(AsyncWebServerRequest * request);
	void handleUpd(AsyncWebServerRequest * request);
	void handleTest(AsyncWebServerRequest * request);
	// void WiFiconnect();
	// void WiFiReconnect();
	void handleLog(AsyncWebServerRequest * request);
	//void handleLogData(AsyncWebServerRequest * request);
	//void handleDistill();
	void handleNotFound(AsyncWebServerRequest * request);
	void handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
	void handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
	void handleBootstrapCss(AsyncWebServerRequest * request);
	void handleFontAwesomeCss(AsyncWebServerRequest * request);
	void handleRadioCss(AsyncWebServerRequest * request);
	void handleBootstrapJs(AsyncWebServerRequest * request);
	void handleJqueryJs(AsyncWebServerRequest * request);

	void handleMainFile(AsyncWebServerRequest * request);
	void handleMainSetup(AsyncWebServerRequest * request);
	void handleW2A(AsyncWebServerRequest * request);
	void handleA2W(AsyncWebServerRequest * request);

	


	//void handleDistillSet();
	//void handleRectify();
	//void handleRectifySet();
	//void handleSuvid();
	//void handleSuvidSet();
	//void handleBrewingSet();
	//void handleBrewing();
	//void handleSuvidChart();
	//Config * conf;
	AsyncWebServer * server;
	//ESP8266HTTPUpdateServer * httpUpdater;
	//ESP8266WebSpiffsUpdater * httpSpiffsUpdater;
	//DataSrc * ds;
	//Hardware * hard;
	//Aggregates * agg;
	//Mode * currentMode;
	//std::function<void(void)>  root;
	//std::function<void(void)>  page1;
	//boolean handleFileRead(String path);
	uint8_t counter;
	IRreceiver * irrc;
	AppData * data;
	boolean in_update;
	void var(String n,String v);
	void var_log(String n,String v);
	String text(String id, String label);
	String checkbox(String id, String label);
};
#endif