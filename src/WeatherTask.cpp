#include "WeatherTask.h"
#include <ArduinoJson.h>

static const char *TAG = "WEATHER";

void WeatherTask::setup()
{

  memset(&config, 0, sizeof(esp_http_client_config_t));
  config.host = "api.openweathermap.org";
  config.port = 80;
  config.query = "q=Irkutsk&appid=7d7b4c4884dc5a5c839debe4488d1712&units=metric";
  config.path = "/data/2.5/weather";
  config.method = HTTP_METHOD_GET;
  config.user_agent = "ESP32";
  config.transport_type = HTTP_TRANSPORT_OVER_TCP;
  config.is_async = false;
  config.timeout_ms = 60000;
  config.disable_auto_redirect = false;
  config.max_redirection_count = 0;
  config.user_data = this;
  config.event_handler = static_weather_event_handler;
}

void WeatherTask::loop()
{
  xEventGroupWaitBits(flg, FLAG_WIFI, false, false, portMAX_DELAY);
  requestURL();
  vTaskDelay(pdMS_TO_TICKS(WEATHER_INTERVAL));
}

esp_err_t WeatherTask::weather_event_handler(esp_http_client_event_t *evt)
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
    ESP_LOGI(TAG, "Len=%d Data |%s|", len, buffer);
    sent();
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

esp_err_t WeatherTask::static_weather_event_handler(esp_http_client_event_t *evt)
{
  WeatherTask *wt = static_cast<WeatherTask *>(evt->user_data);
  return wt->weather_event_handler(evt);
}

void WeatherTask::sent()
{
  event_t ev;
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, buffer);
  if (error)
  {
    ESP_LOGE(TAG, "Deserilization error: %s", error.c_str());
    return;
  }

  if (!doc.isNull())
  {
    int8_t temp = rint((double)doc["main"]["temp"]);
    uint8_t hum = doc["main"]["humidity"];
    uint16_t press = doc["main"]["pressure"];
    ev.button = 10;
    ev.state = ALARM_EVENT;
    ev.data = (hum << 24) & 0xFF000000 | (temp << 16) & 0x00FF0000 | press & 0xFFFF;
    xQueueSend(que, &ev, 0);
    // xMessageBufferSend(_mess, &ev, sizeof(event_t), portMAX_DELAY);
  }
  doc.clear();
}

void WeatherTask::cleanup()
{
  if (buffer != NULL)
  {
    free(buffer);
    buffer = NULL;
  }
}

void WeatherTask::requestURL()
{
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_err_t err;

  if (client)
  {

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
