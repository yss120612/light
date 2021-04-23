#include "MQTTclient.h"
#include "Log.h"

MqttClient::MqttClient()
{
}

MqttClient::~MqttClient()
{
  delete (client);
  delete (wf);
}

void MqttClient::setup(AppData *ad)
{
  wf = new WiFiClient();
  data = ad;
  client = new PubSubClient(mqtt_server, mqtt_port, std::bind(&MqttClient::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), *wf);
  ignore_next_valve=false;
}

String MqttClient::getStatus()
{

  String result="";
  if (client->connected()){
    result+="Mqtt client conected to ";
    result+=mqtt_server;
  }else{
    result+="Mqtt client disconnected!";
    
  }

  return result;
  
}

void MqttClient::callback(char *topic, byte *payload, unsigned int length)
{
  String mess = "";
  String top = topic;
  for (int i = 0; i < length; i++)
  {
    mess += (char)payload[i];
  }
  logg.logging("[" + String(topic) + String("] = ") + mess+String(ignore_next_valve?" (ignor)":""));
  if (top.indexOf("/valve")>0)
  {
    if (ignore_next_valve){
      ignore_next_valve=false;
      return;
    }
    if (mess.equals("0")){}
      //ws_sys->close_valve();
    //else
     // ws_sys->open_valve();
  }
  else if (top.indexOf("/switch")>0)
  {
    //ws_sys->switch_valve();
  }
  else if (top.indexOf("/disalarm")>0)
  {
   // ws_sys->disalarm();
  }
}

void MqttClient::reconnect()
{
  // Loop until we're reconnected
  uint8_t err = 0;
  while (!client->connected())
  {
    logg.logging("Attempting MQTT connection...");
    // Attempt to connect
    if (client->connect("ESP32Client-30", mqtt_user, mqtt_pass))
    {
      logg.logging("connected");
      // Once connected, publish an announcement...
      //client->publish("user/yss1/161/alarm","0");
      
      // ... and resubscribe
      //logg.logging(mqtt_str_valve);
      //client->subscribe(mqtt_str_valve);
      //client->subscribe(mqtt_str_switch);
      //client->subscribe(mqtt_str_disalarm);
      //setValve(ws_sys->valve_is_open());
      //alarm();
    }
    else
    {
      logg.logging("failed, rc=" + client->state() + String(" try again in 5 seconds"));
      err++;
      delay(5000);
    }
    if (err > 2)
      break;
  }
}

void MqttClient::log(String s){
client->publish(mqtt_str_log, s.c_str());
}

void MqttClient::loop(long ms)
{
  client->loop();
  if (ms - last_check < check_time)
    return;
  last_check = ms;
  if (!client->connected())
    reconnect();
};