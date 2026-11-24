# Template-project

`main_wifi_template` is het startpunt voor nieuwe firmware. Het combineert de
access-point- en station-varianten in één project:

- WiFi-modus (AP of STA) en alle WiFi-parameters stel je in via `platformio.ini`;
- de webserver serveert een lege pagina, die je zelf invult;
- eigen FreeRTOS-taken zet je in `application_tasks.cpp`.

## Bestanden

```text
main_wifi_template/
├── platformio.ini           environments voor AP en STA, WiFi-parameters
└── src/
    ├── main.cpp             setup(): initialiseert board, WiFi en applicatietaken
    ├── wifi_app.cpp/.h      WiFi (AP of STA) en webserver
    └── application_tasks.cpp/.h   eigen applicatietaken
```

## WiFi-modus kiezen

`platformio.ini` heeft twee environments:

| Environment | Modus | Gedrag |
| --- | --- | --- |
| `waveshare_esp32s3_ap` | Access point | Het board maakt een eigen netwerk. Open `http://192.168.4.1` (of het ingestelde IP-adres). |
| `waveshare_esp32s3_sta` | Station | Het board verbindt met een bestaand netwerk. Het IP-adres (DHCP) verschijnt in de seriële monitor. Bij verbindingsverlies wordt automatisch opnieuw verbonden. |

Je kiest de modus met `default_envs` in de sectie `[platformio]`, via de
PlatformIO-statusbalk in VS Code of met `pio run -e <environment>`.

## WiFi-parameters

```ini
[env:waveshare_esp32s3_ap]
custom_wifi_ssid           = ESP32-S3-POE-ETH-8DI-8DO
custom_wifi_password       = waveshare
custom_wifi_ap_ip          = 192,168,4,1
custom_wifi_ap_subnet      = 255,255,255,0
custom_wifi_ap_channel     = 1
custom_wifi_ap_max_clients = 4

[env:waveshare_esp32s3_sta]
custom_wifi_ssid     = YourWiFiSSID
custom_wifi_password = YourWiFiPassword
custom_wifi_hostname = esp32s3-8di-8do
```

| Optie | Modus | Betekenis |
| --- | --- | --- |
| `custom_wifi_ssid` | AP, STA | Netwerknaam (aangemaakt in AP, verbonden in STA) |
| `custom_wifi_password` | AP, STA | Wachtwoord. In AP-mode minimaal 8 tekens, of leeg voor een open netwerk |
| `custom_wifi_ap_ip` | AP | IP-adres en gateway van het board, met komma's: `192,168,4,1` |
| `custom_wifi_ap_subnet` | AP | Subnetmasker, met komma's: `255,255,255,0` |
| `custom_wifi_ap_channel` | AP | WiFi-kanaal (1–13) |
| `custom_wifi_ap_max_clients` | AP | Maximaal aantal gelijktijdige clients (1–10) |
| `custom_wifi_hostname` | STA | Hostnaam richting de router |

PlatformIO zet deze opties om naar build flags (`APP_WIFI_MODE_AP` of
`APP_WIFI_MODE_STA`, `APP_WIFI_SSID`, `APP_WIFI_PASSWORD`, `APP_WIFI_AP_*`,
`APP_WIFI_HOSTNAME`). `wifi_app.h` geeft een duidelijke compileerfout als er geen
modus of allebei de modi gekozen zijn.

## wifi_app

| Symbool | Beschrijving |
| --- | --- |
| `void WIFI_Init()` | Registreert de webpagina's en start de taak `WifiTask` (core 0, prioriteit 3). |
| `void WifiTask(void *parameter)` | AP: start het access point en de webserver. STA: verbindt, start de webserver en verbindt opnieuw bij verbindingsverlies. Verwerkt daarna webverzoeken. |
| `void handleRoot()` | Levert de (lege) HTML-startpagina op `/`. |
| `WebServer server` | De webserver op poort 80. |
| `char ipStr[16]` | Huidig IP-adres als tekst. Wordt ook via Bluetooth verstuurd. |
| `bool WIFI_Connection` | AP: `true` zodra het AP actief is. STA: `true` zolang er verbinding is. |

De RGB-led geeft de status aan: groen bij een actief AP of een geslaagde
verbinding, rood knipperend tijdens het verbinden in STA-mode.

### Eigen webpagina's toevoegen

1. Zet de inhoud van de startpagina in `handleRoot()` in `wifi_app.cpp`.
2. Schrijf een handler voor elke extra URL en registreer die in `Web_Register()`:

```cpp
static void handleStatus() {
  server.send(200, "application/json", "{\"din1\":" + String(DIN_Flag[0]) + "}");
}

static void Web_Register() {
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.onNotFound(handleNotFound);
}
```

## application_tasks

`StartApplicationTasks()` wordt aan het einde van `setup()` aangeroepen en start
de eigen taken. Het voorbeeld `MyTask` draait elke seconde:

```cpp
void MyTask(void *pvParameters)
{
  for (;;)
  {
    // bijvoorbeeld: korte piep zolang ingang 1 actief is
    if (DIN_Flag[0])
      Buzzer_Open_Time(100, 0);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
```

:::{tip}
Geef taken die echt werk doen minimaal prioriteit 1 (`tskIDLE_PRIORITY + 1`).
Met prioriteit 0 draait een taak op hetzelfde niveau als de idle-taak.
:::

## Opstartvolgorde

`setup()` in `main.cpp` initialiseert de modules in deze volgorde:

1. `Flash_test()`: toont de flashgrootte
2. `GPIO_Init()`: RGB-led en zoemer
3. `I2C_Init()` en `RTC_Init()`: I²C-bus, PCF85063 en timer-events
4. `RS485_Init()` en `CAN_Init()`
5. `SD_Init()`
6. `WIFI_Init()`: AP of STA
7. `Bluetooth_Init()`
8. `ETH_Init()`: W5500 Ethernet en NTP-tijd
9. `DIN_Init()` en `Dout_Init()`
10. `StartApplicationTasks()`

`loop()` is leeg: al het werk gebeurt in FreeRTOS-taken.
