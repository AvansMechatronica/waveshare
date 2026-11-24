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
