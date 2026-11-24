# Projecten

De workspace bevat vier firmwareprojecten. Ze delen allemaal de board-drivers uit
`libraries/waveshare_board`.

| Project | Netwerk | Kenmerken |
| --- | --- | --- |
| `main_wifi_template` | WiFi AP **of** STA, kiesbaar in `platformio.ini` | Startpunt voor nieuwe firmware. Lege webpagina en een plek voor eigen FreeRTOS-taken. |
| `main_wifi_app` | WiFi access point | Board maakt een eigen netwerk. Webinterface op `http://192.168.4.1`. |
| `main_wifi_sta` | WiFi station + Ethernet | Board verbindt met een bestaand netwerk en toont de webinterface op het verkregen IP-adres. |
| `main_mqtt_all` | WiFi station + Ethernet + MQTT | Als `main_wifi_sta`, aangevuld met MQTT naar de Waveshare cloud. |

## Bestanden per project

| Bestand | Inhoud |
| --- | --- |
| `src/main.cpp` / `src/MAIN_*.cpp` | `setup()` en `loop()`; initialiseert alle modules |
| `src/wifi_app.cpp` / `src/wifi_app.h` | WiFi (AP of STA) en de webserver |
| `src/WS_MQTT.cpp` / `src/WS_MQTT.h` | Alleen `main_mqtt_all`: MQTT-client en brokerinstellingen |
| `src/application_tasks.cpp` / `.h` | Alleen het template: eigen applicatietaken |
| `platformio.ini` | Board-, build- en library-instellingen |

## Configuratie

| Project | WiFi-instellingen | Overige instellingen |
| --- | --- | --- |
| `main_wifi_template` | `platformio.ini` (`custom_wifi_*`) | zie [Template-project](template.md) |
| `main_wifi_app` | `src/wifi_app.h` (`APSSID`, `APPSK`) | |
| `main_wifi_sta` | `src/wifi_app.h` (`STASSID`, `STAPSK`) | |
| `main_mqtt_all` | `src/wifi_app.h` (`STASSID`, `STAPSK`) | `src/WS_MQTT.h` (`MQTT_Server`, `MQTT_Port`, `MQTT_ID`, `MQTT_Pub`, `MQTT_Sub`) |

:::{warning}
De webinterfaces kunnen fysieke uitgangen schakelen. Houd het board op een
vertrouwd netwerk en zet geen echte wachtwoorden in een publieke repository.
:::

## Webinterface (app, sta en mqtt)

De volledige firmwarevarianten bieden via de webserver onder meer:

- uitgangen CH1–CH8 schakelen (`/Switch1` … `/Switch8`, `/AllOn`, `/AllOff`);
- status van uitgangen en ingangen opvragen (`/getData`);
- tijdgestuurde (RTC) events aanmaken en verwijderen;
- RS485- en CAN-data versturen en ontvangen, en de baudrate/bitrate instellen.

Het template heeft deze pagina's niet. Daar serveert de webserver een lege pagina.
