#include "wifi_app.h"

// WiFi parameters, set in platformio.ini
const char *ssid     = APP_WIFI_SSID;
const char *password = APP_WIFI_PASSWORD;

char ipStr[16] = "0.0.0.0";
WebServer server(80);
bool WIFI_Connection = 0;

/*************************************************************  Web pages  *************************************************************/
// Empty start page: add your own content here.
void handleRoot() {
  static const char page[] =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset=\"utf-8\">"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "<title>ESP32-S3-POE-ETH-8DI-8DO</title>"
    "</head>"
    "<body>"
    "</body>"
    "</html>";
  server.send(200, "text/html", page);
}

static void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

static void Web_Register() {
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
}

static void Store_IP(IPAddress myIP) {
  snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
}

/*************************************************************  WiFi  *************************************************************/
void WIFI_Init()
{
  Web_Register();
  xTaskCreatePinnedToCore(
    WifiTask,
    "WifiTask",
    4096,
    NULL,
    3,
    NULL,
    0
  );
}

#ifdef APP_WIFI_MODE_AP
/*  Access-point mode: create our own network and serve the web page  */
void WifiTask(void *parameter) {
  IPAddress apIP(APP_WIFI_AP_IP);
  IPAddress apSubnet(APP_WIFI_AP_SUBNET);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, apSubnet);                          // IP address, gateway and subnet of the AP
  while (!WiFi.softAP(ssid, password, APP_WIFI_AP_CHANNEL, 0, APP_WIFI_AP_MAX_CLIENTS)) {
    printf("Soft AP creation failed, retrying...\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  vTaskDelay(pdMS_TO_TICKS(100));
  RGB_Open_Time(0, 60, 0, 1000, 0);

  Store_IP(WiFi.softAPIP());
  WIFI_Connection = 1;
  printf("WiFi AP \"%s\" started, IP address: %s\r\n", ssid, ipStr);

  server.begin();
  printf("Web server started\r\n");

  while (1) {
    server.handleClient();                                          // Process requests from clients
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  vTaskDelete(NULL);
}

#else
/*  Station mode: connect to an existing network, reconnect when the link drops  */
void WifiTask(void *parameter) {
  uint8_t Count = 0;
  bool serverStarted = false;

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(APP_WIFI_HOSTNAME);
  WiFi.setSleep(true);
  WiFi.begin(ssid, password);                                       // Connect to the specified WiFi network
  printf("Connecting to WiFi \"%s\"", ssid);

  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      WIFI_Connection = 0;
      printf(".");
      RGB_Open_Time(50, 0, 0, 500, 0);
      Count++;
      if (Count >= 10) {                                            // No connection after ~10 s: restart the WiFi stack
        Count = 0;
        printf("\r\n");
        WiFi.disconnect();
        vTaskDelay(pdMS_TO_TICKS(100));
        WiFi.mode(WIFI_OFF);
        vTaskDelay(pdMS_TO_TICKS(100));
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(APP_WIFI_HOSTNAME);
        vTaskDelay(pdMS_TO_TICKS(100));
        WiFi.begin(ssid, password);
      }
    }
    else {
      Count = 0;
      WIFI_Connection = 1;
      Store_IP(WiFi.localIP());
      printf("\r\nWiFi connected, IP address: %s\r\n", ipStr);
      RGB_Open_Time(0, 50, 0, 1000, 0);

      if (!serverStarted) {
        server.begin();
        serverStarted = true;
        printf("Web server started\r\n");
      }

      while (WiFi.status() == WL_CONNECTED) {
        server.handleClient();                                      // Process requests from clients
        vTaskDelay(pdMS_TO_TICKS(10));
      }
      printf("WiFi connection lost, reconnecting");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  vTaskDelete(NULL);
}
#endif
