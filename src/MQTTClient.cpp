#include "MQTTclient.h"
#include "Log.h"
#include "Data.h"

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
  client = new PubSubClient(mqtt_server, mqtt_port, std::bind(&MqttClient::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), * wf) ;
  //client = new PubSubClient(mqtt_server, mqtt_port,  wf);
  ignore_relay1=false;
  ignore_relay2=false;
  ignore_relay3=false;
  ignore_relay4=false;
  ignore_cw=false;
  ignore_ww=false;
  ignore_nw=false;
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

void MqttClient::callback(char *topic, uint8_t *payload, unsigned int length)
{
  
  
  String mess = "";
  String top = topic;
  for (int i = 0; i < length; i++)
  {
    mess += (char)payload[i];
  }
  
  if (top.indexOf("/rel3")>0)
  {
    if (ignore_relay3){ignore_relay3=false;mess+="(ignor)";}
    else {data->putWebEvent(PULT_3,mess.equals("1"));mess+="Put in MQTT";};
  }
  else if (top.indexOf("/rel2")>0)
  {
    if (ignore_relay2){ignore_relay2=false;mess+="(ignor)";}
    else {data->putWebEvent(PULT_2,mess.equals("1"));mess+="Put in MQTT";};
  }
  else if (top.indexOf("/rel1")>0)
  {
    if (ignore_relay1){ignore_relay1=false;mess+="(ignor)";}
    else {data->putWebEvent(PULT_1,mess.equals("1"));mess+="Put in MQTT";};
  }
  else if (top.indexOf("/ww")>0)
  {
    if (ignore_ww){ignore_ww=false;mess+="(ignor)";}
    else {data->putWebEvent(WEB_CANNEL_WW,mess.toInt());mess+="Put in MQTT";};
   // ws_sys->disalarm();
  }
  else if (top.indexOf("/cw")>0)
  {
    if (ignore_cw){ignore_cw=false;mess+="(ignor)";}
    else {data->putWebEvent(WEB_CANNEL_CW,mess.toInt());mess+="Put in MQTT";};
  }
  else if (top.indexOf("/nw")>0)
  {
    if (ignore_nw){ignore_nw=false;mess+="(ignor)";}
    else {data->putWebEvent(WEB_CANNEL_NW,mess.toInt());mess+="Put in MQTT";};
  }
  logg.logging("[" + String(topic) + String("] = ") + mess);
  
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
      client->unsubscribe(mqtt_str_light);
      client->unsubscribe(mqtt_str_notebook);
      client->unsubscribe(mqtt_str_heater);
      client->unsubscribe(mqtt_str_ww);
      client->unsubscribe(mqtt_str_nw);
      client->unsubscribe(mqtt_str_cw);
      // Once connected, publish an announcement...
      client->publish(mqtt_str_notebook,data->isOn(0)?"1":"0");
      client->publish(mqtt_str_heater,data->isOn(1)?"1":"0");
      client->publish(mqtt_str_light,data->isOn(2)?"1":"0");
      client->publish(mqtt_str_ww,String(data->getWW()).c_str());
      client->publish(mqtt_str_nw,String(data->getNW()).c_str());
      client->publish(mqtt_str_cw,String(data->getCW()).c_str());
      
      
      
      // ... and resubscribe
      //logg.logging(mqtt_str_valve);
      client->subscribe(mqtt_str_light);
      client->subscribe(mqtt_str_notebook);
      client->subscribe(mqtt_str_heater);
      client->subscribe(mqtt_str_ww);
      client->subscribe(mqtt_str_nw);
      client->subscribe(mqtt_str_cw);
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

void MqttClient::updateState(uint8_t wh, uint8_t val){
  if (!client->connected()) return;
     
     switch (wh)
      {
      case PULT_1:
        client->publish(mqtt_str_notebook,data->isOn(0)?"1":"0");
        ignore_relay1=true;
        break;
      case PULT_2:
        client->publish(mqtt_str_heater,data->isOn(1)?"1":"0");
        ignore_relay2=true;
        break;
      case PULT_3:
        client->publish(mqtt_str_light,data->isOn(2)?"1":"0");
        ignore_relay3=true;
        break;
      case PULT_4:
        ignore_relay4=true;
        break;
      case CANNEL_CW+100:
        client->publish(mqtt_str_cw,String(data->getCW()).c_str());
        ignore_cw=true;
        break;
      case CANNEL_NW+100:
        client->publish(mqtt_str_nw,String(data->getNW()).c_str());
        ignore_nw=true;
        break;
      case CANNEL_WW+100:
        client->publish(mqtt_str_ww,String(data->getWW()).c_str());
        ignore_ww=true;
        break;
      }
}


void MqttClient::log(String s){
if (!client->connected()) return;
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