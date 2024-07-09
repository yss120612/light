#ifndef _TELEGRAMTASK_h
#define _TELEGRAMTASK_h

#include <Task.h>
#include "Settings.h"
#include "esp_http_client.h"
//#include "Events.h"
//#define BOT_TOKEN "6603332336:AAEIGL9vcbqoJCf8POvsKIM944IK0LLPgBI"
#define API_TELEGRAM_BOT "/bot6603332336:AAEIGL9vcbqoJCf8POvsKIM944IK0LLPgBI"
#define API_TELEGRAM_GET_MESSAGE API_TELEGRAM_BOT "/getUpdates"
#define API_TELEGRAM_POST_MESSAGE API_TELEGRAM_BOT "/sendMessage"
#define CHAT_ID "-4020355910"
#define BOT_ID "896798270"
#define API_TELEGRAM_HOST "api.telegram.org"
/* commands for bot 
relay1 - переключить реле 1
relay2 - переключить реле 2
relay3 - переключить реле 3
relay4 - переключить реле 4
*/

static const char telegram_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIBADANBgkqhkiG9w0BAQsFADCBgzELMAkGA1UEBhMCVVMx
EDAOBgNVBAgTB0FyaXpvbmExEzARBgNVBAcTClNjb3R0c2RhbGUxGjAYBgNVBAoT
EUdvRGFkZHkuY29tLCBJbmMuMTEwLwYDVQQDEyhHbyBEYWRkeSBSb290IENlcnRp
ZmljYXRlIEF1dGhvcml0eSAtIEcyMB4XDTA5MDkwMTAwMDAwMFoXDTM3MTIzMTIz
NTk1OVowgYMxCzAJBgNVBAYTAlVTMRAwDgYDVQQIEwdBcml6b25hMRMwEQYDVQQH
EwpTY290dHNkYWxlMRowGAYDVQQKExFHb0RhZGR5LmNvbSwgSW5jLjExMC8GA1UE
AxMoR28gRGFkZHkgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgLSBHMjCCASIw
DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9xYgjx+lk09xvJGKP3gElY6SKD
E6bFIEMBO4Tx5oVJnyfq9oQbTqC023CYxzIBsQU+B07u9PpPL1kwIuerGVZr4oAH
/PMWdYA5UXvl+TW2dE6pjYIT5LY/qQOD+qK+ihVqf94Lw7YZFAXK6sOoBJQ7Rnwy
DfMAZiLIjWltNowRGLfTshxgtDj6AozO091GB94KPutdfMh8+7ArU6SSYmlRJQVh
GkSBjCypQ5Yj36w6gZoOKcUcqeldHraenjAKOc7xiID7S13MMuyFYkMlNAJWJwGR
tDtwKj9useiciAF9n9T521NtYJ2/LOdYq7hfRvzOxBsDPAnrSTFcaUaz4EcCAwEA
AaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwHQYDVR0OBBYE
FDqahQcQZyi27/a9BUFuIMGU2g/eMA0GCSqGSIb3DQEBCwUAA4IBAQCZ21151fmX
WWcDYfF+OwYxdS2hII5PZYe096acvNjpL9DbWu7PdIxztDhC2gV7+AJ1uP2lsdeu
9tfeE8tTEH6KRtGX+rcuKxGrkLAngPnon1rpN5+r5N9ss4UXnT3ZJE95kTXWXwTr
gIOrmgIttRD02JDHBHNA7XIloKmf7J6raBKZV8aPEjoJpL1E/QYVN8Gb5DKj7Tjo
2GTzLH4U/ALqn83/B2gX2yKQOC16jdFU8WnjXzPKej17CuPKf1855eJ1usV2GDPO
LPAvTK33sefOT6jEm0pUBsV/fdUID+Ic/n4XuKxe9tQWskMJDE32p2u0mYRlynqI
4uJEvlz36hz1
-----END CERTIFICATE-----
)EOF";
#define RECV_LENGTH 100

class TelegramTask: public Task{
public:    
TelegramTask(const char *name, uint32_t stack, QueueHandle_t q, EventGroupHandle_t f, MessageBufferHandle_t m):Task(name, stack){que=q;flg=f;_mess=m;};

protected:
void cleanup() override;
void setup() override;
void loop() override;
void receiveTelegram();
void sendTelegram(const char * data);
static esp_err_t static_telegram_event_handler(esp_http_client_event_t *evt);
esp_err_t telegram_event_handler(esp_http_client_event_t *evt);
void telegram_recv();
void telegram_send();
static void timerCallback(TimerHandle_t tm);

QueueHandle_t que;
EventGroupHandle_t flg;
MessageBufferHandle_t _mess;
esp_http_client_config_t config_telegram;
char * buffer;
char  send_buffer[RECV_LENGTH];
int len;
int telegram_ofset;

};


#endif 