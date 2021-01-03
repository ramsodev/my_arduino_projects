#include <WiFi.h>
#include "esp_system.h"

#define DEBUG 1

#if DEBUG
#define LOG(S) Serial.print(S);
#define ERROR(S) Serial.print(S);
#else
#define LOG(S)
#define ERROR(S) Serial.print(S);
#endif

#define WIFI_SSID "ramsonet"
#define WIFI_PASS "year33old"


void setup()
{
  Serial.begin(115200);
  LOG("mac: ");
  LOG(getMacAddress());
  //-- Wifi -------------------------------------------------------------------
  LOG("Connecting to wifi\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  //-- Attente active du wifi --
  int wifiCount = 100;
  while (WiFi.status() != WL_CONNECTED && wifiCount--)
  {
    LOG(".");
    delay(100);
  }
  LOG("\n");
  if (WiFi.status() != WL_CONNECTED)
  {
    // Battery safety, go to sleep if failed
    ERROR("Failed to connect wifi: ");
    ERROR(WiFi.status());
    ERROR("\n");
    return;
  }
  LOG("Connected to wifi, with ip: ");
  LOG(WiFi.localIP());
  LOG("\n");


}

String getMacAddress() {
  uint8_t baseMac[6];
  // Get MAC address for WiFi station
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

void loop()
{

  const char * hostname;
  esp_err_t err = tcpip_adapter_get_hostname(TCPIP_ADAPTER_IF_STA, &hostname);
  LOG(hostname);
  LOG("\n");
  sleep(5000);
}

