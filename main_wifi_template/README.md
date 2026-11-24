# Waveshare ESP32-S3 POE Ethernet 8DI/8DO — Wi-Fi template

Template firmware for the Waveshare ESP32-S3-POE-ETH-8DI-8DO board. It combines
[`main_wifi_app`](../main_wifi_app/) (access point) and
[`main_wifi_sta`](../main_wifi_sta/) (station) into one project. The Wi-Fi mode
and all Wi-Fi parameters are set in `platformio.ini`, and the web server serves
an empty page that you can fill in.

The board drivers (digital I/O, RS485, CAN, Bluetooth LE, RTC, SD, W5500
Ethernet) are the same as in the original projects.

## Layout

- `src/main.cpp`: start-up code.
- `src/wifi_app.cpp` / `src/wifi_app.h`: Wi-Fi (AP or STA) and the web server.
- `../libraries/waveshare_board/src/`: the board drivers (`WS_*`,
  `I2C_Driver`), shared by all projects in the workspace and found through
  `lib_extra_dirs = ../libraries`.

## Choose the Wi-Fi mode

`platformio.ini` has one environment per mode:

| Environment | Mode | Behaviour |
| --- | --- | --- |
| `waveshare_esp32s3_ap` | Access point | The board creates its own network. Open `http://192.168.4.1` (or the configured `custom_wifi_ap_ip`). |
| `waveshare_esp32s3_sta` | Station | The board joins an existing network and prints its DHCP address on the serial monitor. It reconnects automatically. |

Select a mode by setting `default_envs` in the `[platformio]` section, by
picking the environment in the PlatformIO status bar in VS Code, or on the
command line:

```sh
pio run -e waveshare_esp32s3_sta -t upload
pio device monitor -e waveshare_esp32s3_sta
```

## Wi-Fi parameters

Set these in the environment's section of `platformio.ini`:

| Option | Modes | Meaning |
| --- | --- | --- |
| `custom_wifi_ssid` | AP, STA | Network name (created in AP mode, joined in STA mode). |
| `custom_wifi_password` | AP, STA | Network password. In AP mode at least 8 characters, or empty for an open network. |
| `custom_wifi_ap_ip` | AP | IP address of the board, also the gateway, written as `192,168,4,1`. |
| `custom_wifi_ap_subnet` | AP | Subnet mask, written as `255,255,255,0`. |
| `custom_wifi_ap_channel` | AP | Wi-Fi channel (1–13). |
| `custom_wifi_ap_max_clients` | AP | Maximum simultaneous clients (1–10). |
| `custom_wifi_hostname` | STA | Hostname reported to the router. |

These options are turned into `APP_WIFI_*` build flags. `src/wifi_app.h`
stops the build with a clear error when no mode, or both modes, are set.
Do not commit real Wi-Fi passwords.

## Add web pages

`src/wifi_app.cpp` contains `handleRoot()`, which returns an empty HTML page.
Put your page content there, and register extra URLs in `Web_Register()` with
`server.on("/path", handler);`.

The current IP address is available as `ipStr` and the link state as
`WIFI_Connection`; Bluetooth sends `ipStr` to a connected client.
