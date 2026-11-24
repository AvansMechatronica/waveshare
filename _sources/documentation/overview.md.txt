# Inleiding

Deze documentatie beschrijft de PlatformIO-firmware voor het
[Waveshare ESP32-S3-POE-ETH-8DI-8DO](https://www.waveshare.com/esp32-s3-poe-eth-8di-8do.htm)
board. Het board is gebaseerd op de ESP32-S3-WROOM-1U-N16R8 module (16 MB flash,
8 MB PSRAM) en heeft onder andere:

- 8 digitale ingangen (DIN) en 8 digitale uitgangen (Dout, via een TCA9554 I/O-expander)
- RS485 en CAN (TWAI)
- W5500 Ethernet (met PoE-voeding)
- WiFi en Bluetooth LE
- PCF85063 real-time clock (RTC)
- micro-SD-kaartslot
- RGB-led en zoemer

```{figure} ../images/esp32-s3-poe-eth-8di-8do.png
:alt: Waveshare ESP32-S3-POE-ETH-8DI-8DO
:width: 70%

Het ESP32-S3-POE-ETH-8DI-8DO board. Bron:
[Waveshare documentatie](https://docs.waveshare.com/ESP32-S3-POE-ETH-8DI-8DO).
```

## Blokschema

Het blokschema laat zien welke GPIO-pinnen van de ESP32-S3 met welke
onderdelen verbonden zijn. Dezelfde pinnen staan in de headers van de
[Library-referentie](library.md).

```{figure} ../images/esp32-s3-poe-eth-8di-8do_block_diagram.png
:alt: Blokschema van het ESP32-S3-POE-ETH-8DI-8DO board
:width: 100%

Blokschema met de GPIO-toewijzing. Bron:
[Waveshare documentatie](https://docs.waveshare.com/ESP32-S3-POE-ETH-8DI-8DO).
```

| Onderdeel | Aansluiting | Library-module |
| --- | --- | --- |
| Digitale uitgangen 1–8 | TCA9554 EXIO1–EXIO8 (I²C) | `WS_Dout`, `WS_TCA9554PWR` |
| Digitale ingangen 1–8 | GPIO 4–11 | `WS_DIN` |
| I²C (TCA9554, RTC) | SDA GPIO 42, SCL GPIO 41 | `I2C_Driver` |
| RTC PCF85063 | I²C, interrupt GPIO 40 | `WS_PCF85063`, `WS_RTC` |
| RS485 | TX GPIO 17, RX GPIO 18, richting GPIO 21 | `WS_RS485` |
| CAN | TX GPIO 2, RX GPIO 3 | `WS_CAN` |
| W5500 Ethernet | INT 12, MOSI 13, MISO 14, SCLK 15, CS 16 | `WS_ETH` |
| TF-kaart | D0 GPIO 45, CMD GPIO 47, CLK GPIO 48 | `WS_SD` |
| RGB-led | GPIO 38 | `WS_GPIO` |
| Zoemer | GPIO 46 | `WS_GPIO` |
| BOOT-knop | GPIO 0 | |

Meer informatie, zoals schema's, afmetingen en voorbeeldcode, vind je in de
[Waveshare documentatie](https://docs.waveshare.com/ESP32-S3-POE-ETH-8DI-8DO).

## Opbouw van de workspace

```text
waveshare/
├── libraries/
│   ├── waveshare_board/      gedeelde board-drivers (WS_*, I2C_Driver)
│   ├── ArduinoJson/
│   ├── NTPClient/
│   └── PubSubClient/
├── main_wifi_template/       template: AP of STA via platformio.ini, lege webpagina
├── main_wifi_app/            WiFi access-point firmware met volledige webinterface
├── main_wifi_sta/            WiFi station firmware met volledige webinterface
├── main_mqtt_all/            WiFi station + MQTT (Waveshare cloud)
├── documentation/            deze documentatie (Sphinx / GitHub Pages)
└── waveshare.code-workspace  VS Code workspace met alle projecten
```

Alle projecten gebruiken dezelfde board-drivers uit `libraries/waveshare_board`.
PlatformIO vindt deze library via `lib_extra_dirs = ../libraries` in de
`platformio.ini` van elk project. In de `src`-map van een project staan alleen
de projectspecifieke bestanden, zoals `main.cpp` en `wifi_app.cpp/.h`.

## Waar begin ik?

- Nieuw project starten: kopieer `main_wifi_template` en lees
  [Template-project](template.md).
- Bouwen en uploaden: zie [Aan de slag](getting_started.md).
- Overzicht van de bestaande firmwarevarianten: zie [Projecten](projects.md).
- Functies van de board-drivers: zie [Library-referentie](library.md).
