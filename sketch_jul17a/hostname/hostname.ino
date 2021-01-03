#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <Task.h>
#include <WiFi.h>
#include <WiFiEventHandler.h>
#include <tcpip_adapter.h>
#include "esp_log.h"
#include "sdkconfig.h"

extern "C" {
void app_main(void);
}

static WiFi *wifi;




static void helloWorldHandler(HttpRequest* pRequest, HttpResponse* pResponse)
{
pResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
pResponse->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/plain");
pResponse->sendData("Hello back");
pResponse->close();
}

class HttpTask: public Task {
void run(void *data) {
ESP_LOGD("http", "Testing http ...");

HttpServer* pHttpServer = new HttpServer();
pHttpServer->addPathHandler(
HttpRequest::HTTP_METHOD_GET,
"/helloWorld",
helloWorldHandler);
pHttpServer->start(80);


return;
}
};
static HttpTask *http_task;

class MyWiFiEventHandler: public WiFiEventHandler {
public:
esp_err_t staGotIp(system_event_sta_got_ip_t event_sta_got_ip)
{
printf("staGotIp\n");
/*
http_task = new HttpTask();
http_task->setStackSize(12000);
http_task->start();
*/
return ESP_OK;
}

esp_err_t apStart()
{
printf("apStart\n");
esp_err_t err = tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "mytest");
printf("tcpip_adapter_set_hostname err=%d\n", err);
return ESP_OK;
}
};

void app_main(void)
{


MyWiFiEventHandler *eventHandler = new MyWiFiEventHandler();

wifi = new WiFi();
wifi->setWifiEventHandler(eventHandler);
vTaskDelay(10000 / portTICK_PERIOD_MS);

bool is_connected = wifi->connectAP("????", "????");

esp_err_t err = tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "mytest");
printf("tcpip_adapter_set_hostname err=%d\n", err);

const char * hostname;
err = tcpip_adapter_get_hostname(TCPIP_ADAPTER_IF_STA, &hostname);
//if(err)
{
printf("tcpip_adapter_get_hostname err=%d, host=%s\n", err, hostname);
}

test t;
t.execute();

while(1)
{
printf("is_connected=%d\n", (int)is_connected);
vTaskDelay(100 / portTICK_PERIOD_MS);
}
}
