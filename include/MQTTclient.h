#ifndef _MQTTCLIENT_h
#define _MQTTCLIENT_h

#include <Arduino.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
//#include "Config.h"
#include "Data.h"

class AppData;

class MqttClient{
    public:
    MqttClient();
   ~MqttClient();
    void setup(AppData *dt);
    void loop(long ms);
    void log(String s);
    String getStatus();
    
    private:
    const char * mqtt_server = "mqtt.by"; //сервер MQTT
    const int mqtt_port = 1883;     // Порт для подключения к серверу MQTT
    const char *mqtt_user = "yss1"; // Логин от сервер
    const char *mqtt_pass = "bqt3"; // Пароль от сервера
    const char *mqtt_str_log="user/yss1/30/log";
    PubSubClient * client;
    void reconnect();
    void callback(char* topic, byte* payload, unsigned int length);
    WiFiClient * wf;
    AppData * data;
    const int check_time=1000; //every 3 sec
    unsigned long last_check;
    bool ignore_next_valve;

};
#endif