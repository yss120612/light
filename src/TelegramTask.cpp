#include "TelegramTask.h"
#include <ArduinoJson.h>

static const char *TAG = "TELEGRAM";

void TelegramTask::setup()
{
  memset(&config_telegram, 0, sizeof(esp_http_client_config_t));
  config_telegram.host = "api.telegram.org";
  config_telegram.port = 443;
  config_telegram.user_agent = "ESP32";

  config_telegram.is_async = false;
  config_telegram.timeout_ms = 60000;
  config_telegram.disable_auto_redirect = false;
  config_telegram.max_redirection_count = 0;
  config_telegram.user_data = this;
  config_telegram.event_handler = static_telegram_event_handler;

  // config_telegram.cert_pem=api_telegram_start;
  // config_telegram.client_cert_len=api_telegram_end-api_telegram_start;
  config_telegram.cert_pem = telegram_cert;
  config_telegram.transport_type = HTTP_TRANSPORT_OVER_SSL;
  config_telegram.skip_cert_common_name_check = false;
  config_telegram.use_global_ca_store = false;
  config_telegram.crt_bundle_attach = NULL;
  telegram_ofset = 0;
  //_timer = xTimerCreate("TelegaTimer", pdMS_TO_TICKS(20000), pdTRUE, static_cast<void *>(this), timerCallback);
  // xTimerStart(_timer,0);
}

void TelegramTask::loop()
{
  xEventGroupWaitBits(flg, FLAG_WIFI, false, false, portMAX_DELAY);
  memset(send_buffer, 0, RECV_LENGTH);
  if (xMessageBufferReceive(_mess, send_buffer, RECV_LENGTH, pdMS_TO_TICKS(20000)))
  {
    sendTelegram(send_buffer);
  }
  receiveTelegram();
}

void TelegramTask::timerCallback(TimerHandle_t tm)
{
  TelegramTask *lt = static_cast<TelegramTask *>(pvTimerGetTimerID(tm));
  if (lt)
    lt->receiveTelegram();
}

esp_err_t TelegramTask::telegram_event_handler(esp_http_client_event_t *evt)
{
  switch (evt->event_id)
  {
  case HTTP_EVENT_ERROR: // Error
    ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED: // connection success
    ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT: // Send head events
    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER: // Receive head events
    ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
    break;
  case HTTP_EVENT_ON_DATA: // Receive data events
    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    if (buffer == NULL)
    {
      buffer = (char *)calloc(esp_http_client_get_content_length(evt->client) + 1, sizeof(char));
      if (buffer == NULL)
      {
        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
        return ESP_FAIL;
      }
      len = 0;
    }

    if (!esp_http_client_is_chunked_response(evt->client))
    {
      memcpy(buffer + len, evt->data, evt->data_len);
      len += evt->data_len;
    }

    break;
  case HTTP_EVENT_ON_FINISH: // Session Completion Event
    ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
    buffer[len] = 0;
    // ESP_LOGE(TAG,"Len=%d Data |%s|", len, buffer);
    // sent();
    telegram_recv();
    if (buffer != NULL)
    {
      free(buffer);
      buffer = NULL;
    }
    break;
  case HTTP_EVENT_DISCONNECTED: // Disconnect event
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    break;
  }
  return ESP_OK;
}

esp_err_t TelegramTask::static_telegram_event_handler(esp_http_client_event_t *evt)
{
  TelegramTask *wt = static_cast<TelegramTask *>(evt->user_data);
  return wt->telegram_event_handler(evt);
}

void TelegramTask::telegram_recv()
{

  event_t ev;
  JsonDocument doc;
  //ESP_LOGE(TAG, "%s", buffer);
  event_t event;
  event.state=PULT_BUTTON;
  event.count=IR_DEVICE;
  DeserializationError error = deserializeJson(doc, buffer);
  if (error)
  {
    ESP_LOGE(TAG, "Deserilization error: %s", error.c_str());
    return;
  }
  char * txt;
  uint8_t i = 0;
  if (!doc.isNull())
  {
    if (doc["ok"])
    {
      while (doc["result"][i])
      {
        telegram_ofset = doc["result"][i]["update_id"].as<int>();
        uint16_t len =strlen(doc["result"][i]["message"]["text"].as<const char *>());
        txt = (char*)malloc(len+1);
        memset(txt,0,len+1);
        memcpy(txt,doc["result"][i]["message"]["text"].as<const char *>(),len);
        ESP_LOGE(TAG, "ID=%d id_chart=%d Data |%s|", telegram_ofset , doc["result"][i]["message"]["chat"]["id"].as<long>(), txt);
        if (strncmp("/relay1",txt,7)==0){     
          ESP_LOGE(TAG, "RELAY=%d", 1);
          event.button=PULT_1;
          xQueueSend(que,&event,0);
        }else if (strncmp("/relay2",txt,7)==0){
          ESP_LOGE(TAG, "RELAY=%d", 2);
          event.button=PULT_2;
          xQueueSend(que,&event,0);
        }else if (strncmp("/relay3",txt,7)==0){
          ESP_LOGE(TAG, "RELAY=%d", 3);
          event.button=PULT_3;
          xQueueSend(que,&event,0);
        }else if (strncmp("/relay4",txt,7)==0){
          event.button=PULT_4;
          xQueueSend(que,&event,0);
           ESP_LOGE(TAG, "RELAY=%d", 4);
        }
        free(txt);
        i++;
      }
      if (i > 0)
        telegram_ofset++;
    }
    doc.clear();
  }
}

void TelegramTask::cleanup()
{
  // xTimerDelete(_timer, 0);
  if (buffer != NULL)
  {
    free(buffer);
    buffer = NULL;
  }
}

void TelegramTask::receiveTelegram()
{

  String que = "offset=" + String(telegram_ofset);
  config_telegram.query = que.c_str();
  config_telegram.method = HTTP_METHOD_GET;
  config_telegram.path = API_TELEGRAM_GET_MESSAGE;
  // ESP_LOGE(TAG,"url=%s",que.c_str());
  esp_http_client_handle_t client = esp_http_client_init(&config_telegram);
  esp_err_t err;
  // ESP_LOGE(TAG,"Handle inserted!");
  if (client)
  {
    // memset(buffer,0,sizeof(buffer));
    len = 0;
    buffer = NULL;
    err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Client perform failed!");
    }
    esp_http_client_cleanup(client);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Client cleanup failed!");
    }
  }
  else
  {
    ESP_LOGE(TAG, "Client init failed!");
  }
}

void TelegramTask::sendTelegram(const char *data)
{
  // String que="offset="+String(telegram_ofset);
  config_telegram.method = HTTP_METHOD_POST;
  config_telegram.path = API_TELEGRAM_POST_MESSAGE;
  char *buf = malloc_stringf("{\"chat_id\":%s,\"text\":\"%s\"}", CHAT_ID, data);
  if (buf == nullptr)
  {
    ESP_LOGE(TAG, "Error allocate memory!");
    return;
  }
  esp_http_client_handle_t client = esp_http_client_init(&config_telegram);
  esp_err_t err;
  // ESP_LOGE(TAG,"Handle inserted!");
  if (client)
  {
    // memset(buffer,0,sizeof(buffer));
    len = 0;
    buffer = NULL;

    esp_http_client_set_header(client, "Content-Type", "application/json");
    // Добавляем JSON в тело запроса

    ESP_LOGE(TAG, "%s", buf);
    esp_http_client_set_post_field(client, buf, strlen(buf));
    err = esp_http_client_perform(client);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Client perform failed!");
    }
    esp_http_client_cleanup(client);
    free(buf);
    if (err != ESP_OK)
    {
      ESP_LOGE(TAG, "Client cleanup failed!");
    }
  }
  else
  {
    ESP_LOGE(TAG, "Client init failed!");
  }
}
