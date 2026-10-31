# Waveshare ESP32-S3 I/O firmware workspace

This workspace contains three PlatformIO firmware variants for the
[Waveshare ESP32-S3-POE-ETH-8DI-8DO](https://www.waveshare.com/esp32-s3-poe-eth-8di-8do.htm).
The projects target the ESP32-S3-WROOM-1U N16R8 module (16 MB flash and 8 MB
PSRAM) and provide firmware for the board's digital I/O and communication
interfaces.

## Projects

| Project | Network mode | What makes it different |
| --- | --- | --- |
| [`main_mqtt_all/`](main_mqtt_all/) | Wi-Fi client and Ethernet | Adds MQTT connectivity alongside the web interface. Configure the broker, client, and topics before use. |
| [`main_wifi_app/`](main_wifi_app/) | Wi-Fi access point | Hosts its own Wi-Fi network and web interface. Connect to the configured access point and open `http://192.168.4.1`. |
| [`main_wifi_sta/`](main_wifi_sta/) | Wi-Fi client and Ethernet | Joins an existing Wi-Fi network and serves the web interface on the device's network address. |

The variants share board functionality such as eight digital inputs and
outputs, RS485, CAN, Bluetooth LE, RTC scheduling, SD storage, and W5500
Ethernet support. The exact enabled behavior and setup instructions are
documented in each project's README.

## Workspace layout

- [`waveshare.code-workspace`](waveshare.code-workspace) opens the three
  firmware projects together in Visual Studio Code.
- [`libraries/`](libraries/) contains libraries shared by the projects.
- Each firmware project has its own `platformio.ini`, `src/`, and README.
  PlatformIO dependencies are resolved from that project's configuration.

## Requirements

- A Waveshare ESP32-S3-POE-ETH-8DI-8DO board and USB data cable.
- [PlatformIO Core](https://platformio.org/install/cli) or the PlatformIO IDE
  extension for Visual Studio Code.
- Wi-Fi and/or Ethernet access appropriate to the selected firmware variant.

## Build, upload, and monitor

Run PlatformIO commands from the directory of the firmware variant you want
to use. For example:

```sh
cd main_mqtt_all
pio run -e waveshare_esp32s3_io
pio run -e waveshare_esp32s3_io -t upload
pio device monitor -e waveshare_esp32s3_io
```

The same environment name, `waveshare_esp32s3_io`, is used by all three
projects. The serial monitor runs at 115200 baud. Replace `main_mqtt_all` with
`main_wifi_app` or `main_wifi_sta` to build or upload another variant.

## Configure before deployment

- Review the selected project's `src/WS_Information.h` for network settings.
  The station-mode projects need Wi-Fi credentials; the MQTT variant also
  needs its broker settings.
- The web interfaces can control physical outputs. Keep the device on a
  trusted network and do not expose it directly to the internet.
- Do not commit real Wi-Fi or MQTT credentials.

See each project's README for variant-specific configuration, connection
instructions, and firmware details:

- [MQTT firmware](main_mqtt_all/README.md)
- [Wi-Fi access-point firmware](main_wifi_app/README.md)
- [Wi-Fi station firmware](main_wifi_sta/README.md)
