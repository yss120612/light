
#ifndef _HTTPHELPER_h
#define _HTTPHELPER_h

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>



class HttpHelper
{
public:
	HttpHelper();
	~HttpHelper();

	void clientHandle();
	//boolean handleFileRead(String path);
	void setup();
	//void setDataSource(DataSrc * d) { ds = d; };
	boolean isConnected();
	
	/*void setMode(Mode * m);
	Mode * getMode();*/

private:
	void handleRoot(AsyncWebServerRequest * request);
	void handleUpd(AsyncWebServerRequest * request);
	void WiFiconnect();
	void WiFiReconnect();
	void handleLog(AsyncWebServerRequest * request);
	//void handleDistill();
	void handleNotFound(AsyncWebServerRequest * request);
	void handleUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
	void handleSpiffs(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final);
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

};
#endif