#include "HTTPServer.h"

#include "sys\stat.h"
#include <string.h>
#include "esp_https_ota.h"
#include "esp_ota_ops.h"

const char * HTAG="HTTPTAG";

 void HTTPServer::setup(){
    
    server = NULL;
 }

 void HTTPServer::start(){
    if (server == NULL) {
    ESP_LOGI(HTAG, "Starting webserver");
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    httpd_uri_t uri_index = {
    .uri = "/",
    .method = HTTP_GET,
    .user_ctx = this};

    httpd_uri_t uri_index_2 = {
    .uri = "/index.htm",
    .method = HTTP_GET,
    .user_ctx = this};

    
    httpd_uri_t uri_upd = {
    .uri = "/upd.htm",
    .method = HTTP_GET,
    .user_ctx = this};

    httpd_uri_t uri_reboot = {
    .uri = "/reboot",
    .method = HTTP_GET,
    .user_ctx = this};


    httpd_uri_t uri_bootstrap_css = {
    .uri = "/css/bootstrap.min.css",
    .method = HTTP_GET,
    .user_ctx = this};

    httpd_uri_t uri_bootstrap_js = {
    .uri = "/js/bootstrap.min.js",
    .method = HTTP_GET,
    .user_ctx = this};

    httpd_uri_t uri_jquery_js = {
    .uri = "/js/jquery.min.js",
    .method = HTTP_GET,
    .user_ctx = this};

    httpd_uri_t uri_timepicker_js = {
    .uri = "/js/timepicker.min.js",
    .method = HTTP_GET,
    .user_ctx = this};

    httpd_uri_t uri_upload = {
    .uri = "/upload_*",
    .method = HTTP_POST,
    .user_ctx = this};

    //callback my_cb= [this](httpd_req_t* req)->esp_err_t {return this->send_web_page(req);};
    uri_index.handler = &send_web_page_static;
    uri_index_2.handler = &send_web_page_static;
    uri_upd.handler = &send_web_page_static;
    uri_bootstrap_css.handler = &send_web_page_static;
    uri_bootstrap_js.handler = &send_web_page_static;
    uri_jquery_js.handler = &send_web_page_static;
    uri_timepicker_js.handler = &send_web_page_static;
    uri_upload.handler = &send_web_page_static;
    uri_reboot.handler = &send_web_page_static;
    uri_upload.handler = &send_web_page_static;//POST request
    uri_upd.handler = &send_web_page_static;

    if (httpd_start(&server, &config) == ESP_OK)    {
      xEventGroupSetBits(flags,FLAG_HTTP);  
      //ESP_LOGE(HTAG, "Registering URI handlers");
      httpd_register_uri_handler(server, &uri_index);
      httpd_register_uri_handler(server, &uri_index_2);
      httpd_register_uri_handler(server, &uri_upd);
      httpd_register_uri_handler(server, &uri_upload);
      httpd_register_uri_handler(server, &uri_reboot);
      httpd_register_uri_handler(server, &uri_bootstrap_css);
      httpd_register_uri_handler(server, &uri_bootstrap_js);
      httpd_register_uri_handler(server, &uri_jquery_js);
      httpd_register_uri_handler(server, &uri_timepicker_js);
           
    }else{
        ESP_LOGI(HTAG, "Failed start http server");
    }
    }
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    //ESP_ERROR_CHECK(nvs_flash_init());

    spiffs_conf.base_path = "/spiffs";
    spiffs_conf.partition_label = NULL;
    spiffs_conf.max_files = 5;
    spiffs_conf.format_if_mount_failed = true;

    ESP_LOGI(HTAG, "esp vfs spiffs register");

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));
    
     
 }

esp_err_t HTTPServer::send_web_page(httpd_req_t *req)
{
    String fname="/spiffs";
    //ESP_LOGE(HTAG,"uri=%s",req->uri);
    if (strcmp("/",req->uri)==0){
        fname+="/index.htm";
    }
    else{
        fname+=req->uri;
    }
    
    memset((void *)buffer, 0, sizeof(buffer));
    //ESP_LOGE(HTAG,"uri=%s",req->uri);
    //ESP_LOGE(HTAG,"string=%s",fname.c_str());
    if (fname.indexOf("upload")>0){
        return upload(req);
    }

    if (fname.indexOf("reboot")>0){
        return reboot(req);
    }

    struct stat st;
    char * name=(char *)fname.c_str();
    
    if (stat(name, &st))
    {
        ESP_LOGE(HTAG, "%s not found",name);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not Found");
        return ESP_ERR_NOT_FOUND;
    }
    int remaining=st.st_size;
    
    FILE *fp = fopen(name, "r");
    char *type = http_content_type(name);
    httpd_resp_set_type(req, type);
    //ESP_LOGE(HTAG,"file=%s %d bytes content type=%s",fname.c_str(),remaining,type);

    size_t read_len;
    do {
        read_len = fread(buffer, 1, min(remaining,BUF_LEN), fp);
        if (read_len > 0) {
        httpd_resp_send_chunk(req, buffer, read_len);
        remaining-=read_len;
        }
    } while(remaining>0);
    httpd_resp_send_chunk(req,buffer,0);

    fclose(fp);

    //read_file(fname.c_str());
    //return httpd_resp_send(req, buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t HTTPServer::send_web_page_static(httpd_req_t *req)
{

    //ESP_LOGE(HTAG,"uri=%s",req->uri);
    HTTPServer * hts= static_cast<HTTPServer *>(req->user_ctx);
    req->user_ctx=NULL;
    //esp_https_ota_config_t ota_config;
    
    

    //esp_https_ota(&ota_config);
    
    return hts->send_web_page(req);
    
}

// void HTTPServer::read_file(const char * name){
//     memset((void *)buffer, 0, sizeof(buffer));
//     struct stat st;
//     if (stat(name, &st))
//     {
//         ESP_LOGE(HTAG, "%s not found",name);
//         return;
//     }

//     FILE *fp = fopen(name, "r");
    
//     if (fread(buffer, st.st_size, 1, fp) == 0)
//     {
//         ESP_LOGE(HTAG, "fread failed");
//     }
//     fclose(fp);
//     ESP_LOGE(HTAG, "fread %s success %d bytes!",name,st.st_size);
// }    

 void HTTPServer::cleanup(){
    if (server) {
      ESP_LOGE(HTAG, "Stopping webserver");
      //stop_webserver(server);
      httpd_stop(server);
      xEventGroupClearBits(flags,FLAG_HTTP);
      server = NULL;
  }
 }


void HTTPServer::loop(){
    int fl=xEventGroupGetBits(flags);
    if ( fl & FLAG_WIFI && !(fl & FLAG_HTTP)){
        start();
    }
    vTaskDelay(3000);
}

esp_err_t HTTPServer::reboot(httpd_req_t *req)
{
    const esp_partition_t *boot_partition = esp_ota_get_boot_partition();
    sprintf(buffer, "%s\nRebooting...",  boot_partition->label);
    httpd_resp_send(req, buffer, strlen(buffer));
    esp_restart();
}

esp_err_t HTTPServer::upload(httpd_req_t *req){
    String full_path=req->uri;
    
    String err="Filename too long";
    if (full_path.length() >= CONFIG_FATFS_MAX_LFN) {
            ESP_LOGE(HTAG, "%s (%s:%u)",err.c_str(),__FILE__, __LINE__);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err.c_str());
            return ESP_FAIL;
        }
    
    if (full_path.indexOf("html")>0)
    {
        ESP_LOGE(HTAG, "upload html");
        return upload_www(req);
    }
    else if (full_path.indexOf("bin")>0)
    {
        ESP_LOGE(HTAG, "upload bin");
        return upload_bin(req);
    } 
    else 
    {
        err="Invalid path";
        ESP_LOGE(HTAG, "%s: %s. (%s:%u)",err.c_str(), req->uri, __FILE__, __LINE__);
        /* Respond with 400 Bad Request */
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err.c_str());
        return ESP_FAIL;
    }
    return ESP_OK;
}



static int getHeaderEnd(char * buff, int len){
    int pos=0;
    bool check=false;
    int counter=0;
    const char * lf ="\r\n\r\n";
    char le [5];
    le[4]=0;
    for (pos=0;pos<len-strlen(lf);pos++){
        memcpy(le,buff+pos,strlen(lf));
        if (strcmp(le,lf)==0){
            return pos+strlen(lf);
        }
    }
    return 0;
}

static int getDataEnd(char * buff, int len){
    int pos=0;
    bool check=false;
    int counter=0;
    char lf[12] ="\r\n000000Web";
    memset(lf+2,'\x2d',6);
    char le [12];
    le[11]=0;
    for (pos=0;pos<len-strlen(lf);pos++){
        memcpy(le,buff+pos,strlen(lf));
        
        if (strcmp(le,lf)==0){
            return pos;
        }
    }
    return -1;
}


esp_err_t HTTPServer::upload_www(httpd_req_t *req){
    FILE *fp = NULL;
    size_t len,global_cont_len, global_recv_len = 0;
    String err = "Unknown error";
    esp_ota_handle_t ota_handle;
    int data_begin,data_end=-1;
    const esp_partition_t *partition;
    esp_err_t ret = ESP_OK;

    global_cont_len = req->content_len;
    int bytes_read,bytes_write=0;
    bool need_write=true;

    global_cont_len = req->content_len;
    partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
    if (!partition){
        err = "No partiton";
        ESP_LOGE(HTAG, "%s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
        return ESP_FAIL;
    }
    
    ret=esp_partition_erase_range(partition,0,partition->size);
    if (ret != ESP_OK) {
        switch (ret) {
                case ESP_ERR_INVALID_ARG:
                    err = "iterator or dst are NULL";
                    break;
                case ESP_ERR_INVALID_SIZE:
                    err = "erase would go out of bounds of the partition";
                    break;
                default:
                    err = "partition is read-only; or one of error codes from lower-level flash drive";
                    break;
            }
            ESP_LOGE(HTAG, "OTA begin return error. %s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
            return ESP_FAIL; 
            
            } else { //ESP BEGIN IS OK
            bool begin = true;
            while(global_cont_len && data_end<0) {
                 
                if (global_cont_len>=2*SHORT_BUF_LEN){
                    bytes_read=BUF_LEN;
                }else if (global_cont_len<=SHORT_BUF_LEN){
                    bytes_read=global_cont_len;
                }else if (global_cont_len>BUF_LEN){
                    bytes_read=SHORT_BUF_LEN;
                }else if (global_cont_len<=BUF_LEN){
                    bytes_read=global_cont_len;
                }
                len = httpd_req_recv(req, buffer, bytes_read);
                data_begin=0;
                if (len) {
                    if (global_recv_len==0) {
                        data_begin=getHeaderEnd(buffer,len);
                    }
                    bytes_write=len;
                    if (data_begin>0) 
                    {
                        bytes_write-=data_begin;
                    }

                    if (global_cont_len<=BUF_LEN) 
                    {
                        data_end=getDataEnd(buffer+data_begin,len-data_begin);
                        if (data_end>=0) bytes_write=data_end;
                        
                    }else{
                        data_end=-1;
                    }
                    
                    if (need_write) ret = esp_partition_write_raw(partition, global_recv_len,buffer+data_begin,bytes_write);
                    //esp_ota_write(ota_handle, (void*)(buffer+data_begin),bytes_write);
                    if (data_end>=0) need_write=false;
                    if (ret != ESP_OK) {
                        switch (ret) {
                            case ESP_ERR_INVALID_ARG:
                                err = "dst_offset exceeds partition size";
                                break;
                            case ESP_ERR_INVALID_SIZE:
                                err = "write would go out of bounds of the partition";
                                break;
                            default:
                                err = "partition is read-only or one of error codes from lower-level flash driver";
                                break;
                        }

                        ESP_LOGE(HTAG, "SPIFFS write return error. %s. (%s:%d)", err.c_str(), __FILE__, __LINE__);
                        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
                        return ESP_FAIL;
                    }
                    global_recv_len += bytes_write;
                    global_cont_len -= len;
                    printf(".");
                    fflush(stdout);
                }
            }
            
            
            
            // ret = esp_ota_end(ota_handle);
            // if (ret != ESP_OK) {
            //     switch (ret) {
            //         case ESP_ERR_NOT_FOUND:
            //             err = "OTA handle was not found";
            //             break;
            //         case ESP_ERR_INVALID_ARG:
            //             err = "Handle was never written to";
            //             break;
            //         case ESP_ERR_OTA_VALIDATE_FAILED:
            //             err = "OTA image is invalid";
            //             break;
            //         case ESP_ERR_INVALID_STATE:
            //             err = "Internal error writing the final encrypted bytes to flash";
            //     }
            //         }
                
            }
            //esp_partition_read(partition,0,buffer,1024);
            //buffer[1023]=0;
            ESP_LOGE(HTAG,"Binary transferred finished: %d bytes", global_recv_len);
            //ESP_LOGE(HTAG,"Binary transferred finished: %d bytes\n", global_recv_len);
            sprintf(buffer, "Partition `%s` %d bytes uploaded successfully. <a href=\"/index.htm\">Home</a>", partition->label, global_recv_len);
            httpd_resp_send(req, buffer, strlen(buffer));
            return ESP_OK;
//     size_t used;
//     size_t free;
    
//     if (esp_spiffs_info(spiffs_conf.partition_label, &full, &used) != ESP_OK) {
//         free=0;
//     }
//     else{
//         free=full-used;
//     }

//     //return full - used;
//     ESP_LOGE(HTAG, "all=%d free=%d ", full,free);

//     if (free < req->content_len) {
//         err = "Upload file too large";
//         ESP_LOGE(HTAG, "%s free=%d len=%d. (%s:%u)", err.c_str(),free,req->content_len, __FILE__, __LINE__);
//         httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err.c_str());
//         return ESP_FAIL;
//     }
// return ESP_OK;
//     // fp = fopen(tmpname.c_str(), "wb");

//     // if (!fp) {
//     //     err = "Failed to create file";
//     //     ESP_LOGE(HTAG, "%s \"%s\" (%s:%u)", err.c_str(), tmpname, __FILE__, __LINE__);
//     //     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
//     //     return ESP_FAIL;
//     // }

//     // fseek(fp, 0, SEEK_SET);

//     ESP_LOGI(HTAG,"Loading \"%s\" file\n", full_path);
//     ESP_LOGI(HTAG,"Please wait\n");

//     while(global_cont_len) {
//         /* Receive the file part by part into a buffer */
//         memset(buffer,0,size_t(BUF_LEN));
//         received = httpd_req_recv(req, buffer, min(global_cont_len, (size_t)BUF_LEN));
//         if (received <= 0) {
//             if (received == HTTPD_SOCK_ERR_TIMEOUT) {
//                 /* Retry if timeout occurred */
//                 continue;
//             }

//             /* In case of unrecoverable error,
//              * close and delete the unfinished file*/
//             // fclose(fp);
//             // unlink(tmpname.c_str());

//             err = "File reception failed";
//             ESP_LOGE(HTAG, "%s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
//             /* Respond with 500 Internal Server Error */
//             httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
           
//             return ESP_FAIL;
//         }

//         recorded_len += received;
//         ESP_LOGE(HTAG,"%s",buffer);

//         /* Write buffer content to file on storage */
//         // if (received && (received != fwrite(buffer, 1, received, fp))) {
//         //     /* Couldn't write everything to file!
//         //      * Storage may be full? */
//         //     // fclose(fp);
//         //     // unlink(tmpname.c_str());
            

//         //     err = "Failed to write file to storage";
//         //     ESP_LOGE(HTAG, "%s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
//         //     /* Respond with 500 Internal Server Error */
//         //     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
//         //     return ESP_FAIL;
//         // }

//         /* Keep track of remaining size of
//          * the file left to be uploaded */
//         global_cont_len -= received;

//         ESP_LOGE(HTAG,".");
//         //fflush(stdout);
//     }


//     //fclose(fp);

//     //printf("\n");

//     ESP_LOGI("File transferred finished: %d bytes\n", recorded_len);

//     // struct stat st;
//     // if (stat(newname.c_str(), &st) == 0) {
//     //     unlink(newname.c_str());
//     // }

//     // if (rename(tmpname.c_str(), newname.c_str()) != 0) {
//     //     ESP_LOGE(HTAG, "File rename \"%s\" to \"%s\" failed. (%s:%u)", tmpname, newname, __FILE__, __LINE__);
//     //     httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to rename file");
//     //     // free(newname);
//     //     // free(tmpname);
//     //     return ESP_FAIL;
//     // }

    

}



esp_err_t HTTPServer::upload_bin(httpd_req_t *req){

const esp_partition_t *partition;
    esp_ota_handle_t ota_handle;
    esp_err_t ret = ESP_OK;

    size_t global_cont_len;
    size_t len;
    size_t global_recv_len = 0;

    String err = "Unknown error";

    esp_image_header_t          *image_header = NULL;
    esp_app_desc_t              *app_desc = NULL;

    global_cont_len = req->content_len;
    int bytes_read,bytes_write=0;
    bool need_write=true;
    int data_begin,data_end=-1;


    partition = esp_ota_get_next_update_partition(NULL);

    if (partition) {
        if (partition->size < req->content_len) {
            err = "Firmware image too large";
            ESP_LOGE(HTAG, "%s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err.c_str());
            return ESP_FAIL;
        }
        
        ret = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &ota_handle);

        if (ret != ESP_OK) {
            
         switch (ret) {
                case ESP_ERR_INVALID_ARG:
                    err = "Partition or out_handle arguments were NULL, or not OTA app partition";
                    break;
                case ESP_ERR_NO_MEM:
                    err = "Cannot allocate memory for OTA operation";
                    break;
                case ESP_ERR_OTA_PARTITION_CONFLICT:
                    err = "Partition holds the currently running firmware, cannot update in place";
                    break;
                case ESP_ERR_NOT_FOUND:
                    err = "Partition argument not found in partition table";
                    break;
                case ESP_ERR_OTA_SELECT_INFO_INVALID:
                    err = "The OTA data partition contains invalid data";
                    break;
                case ESP_ERR_INVALID_SIZE:
                    err = "Partition doesn�t fit in configured flash size";
                    break;
                case ESP_ERR_FLASH_OP_TIMEOUT:
                case ESP_ERR_FLASH_OP_FAIL:
                    err = "Flash write failed";
                    break;
                case ESP_ERR_OTA_ROLLBACK_INVALID_STATE:
                    err = "The running app has not confirmed state";
                    break;
                default:
                    err = "Unknown error";
                    break;
            }
            ESP_LOGE(HTAG, "OTA begin return error. %s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
            return ESP_FAIL;

        } else { //BEGIN is OK
               while(global_cont_len && data_end<0) {
                if (global_cont_len>=2*SHORT_BUF_LEN){
                    bytes_read=BUF_LEN;
                }else if (global_cont_len<=SHORT_BUF_LEN){
                    bytes_read=global_cont_len;
                }else if (global_cont_len>BUF_LEN){
                    bytes_read=SHORT_BUF_LEN;
                }else if (global_cont_len<=BUF_LEN){
                    bytes_read=global_cont_len;
                }
                
                len = httpd_req_recv(req, buffer, bytes_read);
                data_begin=0;
                
                if (len) {
                    if (global_recv_len==0) {
                        data_begin=getHeaderEnd(buffer,len);
                        image_header = (esp_image_header_t*)(buffer+data_begin);
                        app_desc = (esp_app_desc_t*)(buffer +
                                    sizeof(esp_image_header_t) +
                                    sizeof(esp_image_segment_header_t)+data_begin);
                        if (image_header->magic != ESP_IMAGE_HEADER_MAGIC ||
                            app_desc->magic_word != ESP_APP_DESC_MAGIC_WORD) {
                            err = "Invalid flash image type ";
                            ESP_LOGE(HTAG, "%s. (%s:%u) MAGIC_H:%X MAGIC_D:%X data_begin=%d", err.c_str(), __FILE__, __LINE__,image_header->magic,app_desc->magic_word,data_begin);
                            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, err.c_str());
                            vTaskDelay(3000 / portTICK_PERIOD_MS);
                            return ESP_FAIL;
                        }
                        
                        
                        ESP_LOGI(HTAG,"Uploading image file \"%s\"", req->uri);
                        ESP_LOGI(HTAG,"Image project name \"%s\"", app_desc->project_name);
                        ESP_LOGI(HTAG,"Compiled %s %s", app_desc->time, app_desc->date);
                        ESP_LOGI(HTAG,"IDF version %s", app_desc->idf_ver);
                        ESP_LOGI(HTAG,"Writing to partition name \"%s\" subtype %d at offset 0x%x",
                              partition->label, partition->subtype, partition->address);
                        ESP_LOGI(HTAG,"Please wait...");
                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                     }
                    bytes_write=len;
                    if (data_begin>0) 
                    {
                        bytes_write-=data_begin;
                    }

                    if (global_cont_len<=BUF_LEN) 
                    {
                        data_end=getDataEnd(buffer+data_begin,len-data_begin);
                        if (data_end>=0) bytes_write=data_end;
                        
                    }else{
                        data_end=-1;
                    }
                    
                    if (need_write) ret = esp_ota_write(ota_handle, (void*)(buffer+data_begin),bytes_write);
                    if (data_end>=0) need_write=false;
                    
                    if (ret != ESP_OK) {
                        switch (ret) {
                            case ESP_ERR_INVALID_ARG:
                                err = "Handle is invalid";
                                break;
                            case ESP_ERR_OTA_VALIDATE_FAILED:
                                err = "First byte of image contains invalid app image magic byte";
                                break;
                            case ESP_ERR_FLASH_OP_TIMEOUT:
                            case ESP_ERR_FLASH_OP_FAIL:
                                err = "Flash write failed";
                                break;
                            case ESP_ERR_OTA_SELECT_INFO_INVALID:
                                err = "OTA data partition has invalid contents";
                                break;
                            default:
                                err = "Unknown error";
                                break;
                        }

                        ESP_LOGE(HTAG, "OTA write return error. %s. (%s:%d)", err.c_str(), __FILE__, __LINE__);
                        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
                        return ESP_FAIL;
                    }
                    global_recv_len += bytes_write;
                    global_cont_len -= len;
                    printf(".");
                    fflush(stdout);
                }
            }
            
            ESP_LOGE(HTAG,"Binary transferred finished: %d bytes\n", global_recv_len);
            
            ret = esp_ota_end(ota_handle);
            if (ret != ESP_OK) {
                switch (ret) {
                    case ESP_ERR_NOT_FOUND:
                        err = "OTA handle was not found";
                        break;
                    case ESP_ERR_INVALID_ARG:
                        err = "Handle was never written to";
                        break;
                    case ESP_ERR_OTA_VALIDATE_FAILED:
                        err = "OTA image is invalid";
                        break;
                    case ESP_ERR_INVALID_STATE:
                        err = "Internal error writing the final encrypted bytes to flash";
                }

                ESP_LOGE(HTAG, "OTA end return error. %s. (%s:%d)", err.c_str(), __FILE__, __LINE__);
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
                return ESP_FAIL;
            }
            ret = esp_ota_set_boot_partition(partition);
            if (ret != ESP_OK) {
                err = "Set boot partition is error";
                ESP_LOGE(HTAG, "%s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
                return ESP_FAIL;
            }

            sprintf(buffer, "File `%s` %d bytes uploaded successfully.\nNext boot partition is %s.\nRestart system.. <a href=\"/index.htm\">Home</a>", req->uri, global_recv_len, partition->label);
            httpd_resp_send(req, buffer, strlen(buffer));
            vTaskDelay(pdMS_TO_TICKS(3000));
            esp_restart();  
           
        }


    } else {
        err = "No partiton";
        ESP_LOGE(HTAG, "%s. (%s:%u)", err.c_str(), __FILE__, __LINE__);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, err.c_str());
        return ESP_FAIL;
    }

    return ESP_OK;
}