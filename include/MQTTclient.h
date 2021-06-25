#ifndef _MQTTCLIENT_h
#define _MQTTCLIENT_h

#include <Arduino.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
//#include "Config.h"
//#include "Data.h"

class AppData;

class MqttClient{
    public:
    MqttClient();
   ~MqttClient();
    void setup(AppData *dt);
    void loop(long ms);
    void log(String s);
    String getStatus();
    void updateState(uint8_t wh, uint8_t val);

    private:
    const char * mqtt_server = "mqtt.by"; //сервер MQTT
    const int mqtt_port = 1883;     // Порт для подключения к серверу MQTT
    const char *mqtt_user = "yss1"; // Логин от сервер
    const char *mqtt_pass = "bqt3"; // Пароль от сервера
    
    const char *mqtt_str_log="user/yss1/30/log";
    const char *mqtt_str_notebook="user/yss1/30/rel1";
    const char *mqtt_str_heater="user/yss1/30/rel2";
    const char *mqtt_str_light="user/yss1/30/rel3";
    const char *mqtt_str_ww="user/yss1/30/ww";
    const char *mqtt_str_nw="user/yss1/30/nw";
    const char *mqtt_str_cw="user/yss1/30/cw";
    boolean ignore_relay1;
    boolean ignore_relay2;
    boolean ignore_relay3;
    boolean ignore_relay4;
    boolean ignore_cw;
    boolean ignore_ww;
    boolean ignore_nw;
    PubSubClient * client;
    void reconnect();
    void callback(char* topic, byte* payload, unsigned int length);
    WiFiClient * wf;
    AppData * data;
    const int check_time=1000; //every 3 sec
    unsigned long last_check;
    

};
#endif