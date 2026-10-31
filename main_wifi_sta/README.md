# Waveshare ESP32-S3 POE Ethernet 8DI/8DO

PlatformIO firmware for the Waveshare ESP32-S3-POE-ETH-8DI-8DO board, based on
the ESP32-S3-WROOM-1U with 16 MB flash, 8 MB PSRAM, and a W5500 Ethernet
controller.

## Features

- Eight digital inputs and eight digital outputs. By default, input changes
  control the corresponding output.
- Wi-Fi station connectivity with an HTTP control interface for outputs,
  scheduled events, RS485, and CAN.
- W5500 Ethernet connectivity. When Ethernet obtains an IP address, the
  firmware retrieves network time and sets the PCF85063 RTC.
- RS485, CAN, Bluetooth, SD card, RGB indicator, and buzzer initialization.

## Requirements

- PlatformIO Core or the PlatformIO IDE extension for Visual Studio Code.
- A compatible ESP32-S3-POE-ETH-8DI-8DO board and USB-C data cable.
- Wi-Fi credentials and, when using Ethernet, a network with DHCP and internet
  access for NTP time synchronization.

The project also references shared libraries from `../libraries`; keep that
directory available when building this project.

## Configure Wi-Fi

Set `STASSID` and `STAPSK` in `src/WS_Information.h` to the Wi-Fi network name
and password the board should join. This firmware runs in Wi-Fi station mode;
it does not create an access point. Do not publish or commit real network
credentials.

## Build, upload, and monitor

Run these commands from this project directory:

```sh
pio run -e waveshare_esp32s3_io
pio run -e waveshare_esp32s3_io -t upload
pio device monitor -b 115200
```

The PlatformIO environment uses the Arduino framework, configures the board
for 16 MB flash and OPI PSRAM, and enables USB CDC at boot. Dependencies are
declared in `platformio.ini` and will be resolved by PlatformIO.

## Use

1. Build and upload the firmware.
2. Open the serial monitor at 115200 baud and wait for the board to connect to
   Wi-Fi or Ethernet.
3. Use the IP address printed in the serial output to open the web interface
   in a browser: `http://<device-ip>/`.

The web interface provides output control and pages for RTC events and serial
interfaces. The Ethernet connection is separate from the Wi-Fi connection;
the IP address shown in the serial output identifies the interface that has
connected.

## Project layout

- `src/MAIN_WIFI_STA.cpp` initializes the board peripherals and network
  services.
- `src/WS_*.cpp` and `src/WS_*.h` implement the individual board functions.
- `platformio.ini` defines the PlatformIO board, build settings, and
  dependencies.

## Function reference

### Startup

| Function | Description |
| --- | --- |
| `setup()` | Runs once at startup. Checks flash, initializes GPIO, I2C, RTC, RS485, CAN, SD, Wi-Fi, Bluetooth, Ethernet, digital inputs, and digital outputs. |
| `loop()` | Arduino main loop. It is currently empty; ongoing work is handled by FreeRTOS tasks started by the peripheral modules. |

### Networking and web interface

| Function | Description |
| --- | --- |
| `WIFI_Init()` | Starts the Wi-Fi station task. |
| `WifiStaTask()` | Connects to the configured Wi-Fi network, prints its IP address, starts the HTTP server, and handles requests. It retries the Wi-Fi connection if disconnected. |
| `handleRoot()` | Serves the web interface for output control and the RTC and serial pages. |
| `handleGetData()` | Returns the current digital-output states to the web interface. |
| `handleSwitch()` | Changes an individual output or switches all outputs on or off; the `handleSwitch1()` through `handleSwitch8()` handlers select individual channels. |
| `handleGetRS485Data()` / `handleRS485Send()` | Retrieve received RS485 data and send data over RS485. |
| `handleGetCANData()` / `handleCANSend()` | Retrieve received CAN data and send CAN frames. |
| `ETH_Init()` | Starts the W5500 Ethernet interface and its connection-monitor task. |
| `EthernetTask()` | Monitors Ethernet link state and triggers network-time acquisition when Ethernet connects. |
| `Acquisition_time()` | Gets the current time from NTP and sets the PCF85063 RTC. |

### Inputs and outputs

| Function | Description |
| --- | --- |
| `DIN_Init()` | Initializes the eight digital inputs. With `Dout_Immediate_Default` enabled, input changes control the corresponding output. |
| `Dout_Init()` | Initializes the output expander and starts the output-failure indicator task. |
| `Dout_Open()` / `Dout_Closs()` | Set one output channel high or low. |
| `Dout_CHx_Toggle()` | Toggles one output channel. |
| `Dout_CHx()` / `Dout_CHxs_PinState()` | Set one channel or the eight-channel output bitmask. |
| `Dout_Immediate()` / `Dout_Immediate_CHxn()` / `Dout_Immediate_CHxs()` | Apply immediate output changes to one channel, selected channels, or all channels. |
| `Dout_Analysis()` | Interprets a channel command and applies the requested output action. |
| `GPIO_Init()` | Initializes the RGB indicator and buzzer. |
| `RGB_Light()` / `RGB_Open_Time()` | Set the RGB color continuously or for a timed/flickering interval. |
| `Buzzer_Open()` / `Buzzer_Closs()` / `Buzzer_Open_Time()` | Control the buzzer continuously or for a timed/flickering interval. |

### Serial, CAN, and Bluetooth

| Function | Description |
| --- | --- |
| `RS485_Init()` | Initializes the RS485 interface and its task. |
| `SetData()` / `ReadData()` | Send and receive RS485 data. |
| `RS485_UpdateBaudRate()` | Change the RS485 baud rate. |
| `RS485_Analysis()` | Processes received RS485 commands, including output-control commands. |
| `CAN_Init()` | Initializes the CAN interface and its task. |
| `CAN_Set_Bitrate()` / `CAN_UpdateRate()` | Set or update the CAN bus bitrate. |
| `send_message()` / `send_message_Bytes()` | Send a CAN frame from an ID and payload, or from a prepared TWAI message. |
| `Bluetooth_Init()` | Initializes the Bluetooth Low Energy service and task. |
| `Bluetooth_SendData()` | Send data through the Bluetooth service. |
| `BLE_Set_RTC_Event()` | Passes a Bluetooth command to the RTC-event handling code. |

### RTC and storage

| Function | Description |
| --- | --- |
| `RTC_Init()` | Initializes RTC support and starts the scheduled-event task. |
| `TimerEvent_CHx_Set()` / `TimerEvent_CHxs_Set()` / `TimerEvent_CHxn_Set()` | Schedule a time-based change to one output, an output bitmask, or selected outputs. |
| `TimerEvent_Serial_Set()` | Schedule a serial-interface action. |
| `TimerEvent_printf_ALL()` / `TimerEvent_Del_Number()` | Print configured events or delete an event by number. |
| `PCF85063_Init()` / `PCF85063_Reset()` | Initialize or reset the PCF85063 RTC device. |
| `PCF85063_Set_Time()` / `PCF85063_Set_Date()` / `PCF85063_Set_All()` | Set the RTC time, date, or both. |
| `PCF85063_Read_Time()` | Read the current RTC date and time. |
| `SD_Init()` / `Flash_test()` | Initialize the SD card and report flash size, respectively. |
| `File_Search()` / `Folder_retrieval()` | Search a directory for a named file or collect matching filenames. |

## Network safety

The web interface can operate the board's outputs and has no authentication.
Use it only on a trusted, isolated network; do not expose it directly to the
internet.
