#ifndef _HTTPSERVERTASK_h
#define _HTTPSERVERTASK_h
#include <Task.h>

#include "GlobalSettings.h"
#include <esp_http_server.h>
#include <functional>
#include "esp_spi_flash.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "Settings.h"

//const char * HTAG="HTTPTAG";
#define BUF_LEN 4096
#define BUF_DIF 512
#define SHORT_BUF_LEN BUF_LEN-BUF_DIF

class HTTPServer : public Task{

    public:
    HTTPServer(const char *name, uint32_t stack,EventGroupHandle_t f,QueueHandle_t q,MessageBufferHandle_t voice_message,SystemState_t * ss):Task(name, stack){

        flags = f;
        queue=q;
        st=ss;
        message=voice_message;
    }
    void start();
    protected:
    httpd_handle_t server;
    void setup() override;
    void loop() override;
    void cleanup() override;
    
   // void read_file(const char * name);
    static esp_err_t send_web_page_static(httpd_req_t *req);
    esp_err_t send_web_page(httpd_req_t *req);
    esp_err_t upload(httpd_req_t *req);
    esp_err_t reboot(httpd_req_t *req);
    esp_err_t upload_www(httpd_req_t *req);
    esp_err_t upload_bin(httpd_req_t *req);
     static esp_err_t post_data_static(httpd_req_t *req);//from web to app
    static esp_err_t send_data_static(httpd_req_t *req);//from app to web
    void var(String n, String v);
    esp_err_t post_data(httpd_req_t *req);
    esp_err_t send_data(httpd_req_t * req);
    // void passACallbackToMe(std::function<esp_err_t(httpd_req_t *)> cb) {
	//     // Now invoke (call) the callback
	// 	cb(nullptr);
	// }
    EventGroupHandle_t flags;
    QueueHandle_t queue;
    char buffer[BUF_LEN];
    uint8_t voice_buffer[CMD_BUF_LEN];
    esp_vfs_spiffs_conf_t spiffs_conf;
    SystemState_t * st;
    bool voice_changed;
    String voice_info;
    MessageBufferHandle_t message;
};
#endif