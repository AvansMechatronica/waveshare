# Waveshare ESP32-S3 Wi-Fi I/O Firmware

PlatformIO firmware for the Waveshare ESP32-S3-POE-ETH-8DI-8DO. The ESP32
creates its own Wi-Fi access point and hosts a web interface for controlling
the board's digital outputs, configuring scheduled actions, and using its
communication interfaces.

## Features

- Read eight digital inputs and control eight digital outputs.
- Control outputs from the web interface, Bluetooth LE, and scheduled RTC
  events. Digital-input-to-output control is enabled by default.
- Configure and use RS485 and CAN.
- Use the onboard RTC, SD card, and W5500 Ethernet interface.

## Hardware and requirements

- Waveshare ESP32-S3-POE-ETH-8DI-8DO with an ESP32-S3-WROOM-1U N16R8 module.
- [PlatformIO Core](https://platformio.org/install/cli) or the PlatformIO IDE
  extension for VS Code.
- USB connection to the board for programming and serial monitoring.

The project is configured for the `waveshare_esp32s3_io` PlatformIO environment.
Dependencies declared in `platformio.ini` are installed by PlatformIO.

## Configure and connect

1. Before deployment, set the Wi-Fi access point name and password in
   `src/WS_Information.h`. The repository contains default credentials; replace
   them before using the firmware outside a trusted test environment.
2. Build and upload the firmware using the commands below.
3. Connect a phone or computer to the configured access point.
4. Open **http://192.168.4.1** in a browser to use the web interface.

The access point uses the `192.168.4.1` address and a `/24` subnet. Ethernet is
also initialized; its network address is assigned by the connected network.

## Build, upload, and monitor

Run these commands from this directory:

```sh
pio run -e waveshare_esp32s3_io
pio run -e waveshare_esp32s3_io -t upload
pio device monitor -e waveshare_esp32s3_io
```

The serial monitor is configured for **115200 baud**.

## Project layout

- `platformio.ini` — PlatformIO environment, board settings, and dependencies.
- `src/MAIN_WIFI_AP.cpp` — firmware initialization and main loop.
- `src/WS_WIFI.cpp` — Wi-Fi access point and web interface.
- `src/WS_DIN.cpp`, `src/WS_Dout.cpp` — digital input and output handling.
- `src/WS_RS485.cpp`, `src/WS_CAN.cpp`, `src/WS_Bluetooth.cpp` — communication
  interfaces.
- `src/WS_RTC.cpp`, `src/WS_PCF85063.cpp` — RTC and scheduled events.
- `src/WS_SD.cpp`, `src/WS_ETH.cpp` — SD storage and Ethernet.

At startup, `setup()` initializes the board services and starts their
background tasks. The Arduino `loop()` is empty; ongoing work is handled by
those tasks.

## Function reference

### Application and initialization

- `setup()` in `MAIN_WIFI_AP.cpp` runs the startup sequence: flash check, GPIO
  and I2C setup, RTC, RS485, CAN, SD, Wi-Fi, Bluetooth, Ethernet, digital
  inputs, and digital outputs.
- `loop()` is intentionally empty; module tasks handle ongoing work.
- `GPIO_Init()`, `I2C_Init()`, `RTC_Init()`, `RS485_Init()`, `CAN_Init()`,
  `SD_Init()`, `WIFI_Init()`, `Bluetooth_Init()`, `ETH_Init()`, `DIN_Init()`,
  and `Dout_Init()` initialize their respective hardware or services. Several
  initializers also start a FreeRTOS task.

### Digital inputs and outputs

- `DIN_Read_CH1()` through `DIN_Read_CH8()` read one input. `DIN_Read_CHxs()`
  reads all eight and returns their states as a bitmask, with CH1 in bit 0.
- `DIN_Init()` configures and initializes the inputs; `DINTask()` polls them.
  When `Dout_Immediate_Default` is enabled in `WS_DIN.h`, input changes are
  also applied to the corresponding outputs.
- `Dout_Open(CHx)` and `Dout_Closs(CHx)` set an output high or low.
  `Dout_CHx(CHx, state)` sets an explicit state, while `Dout_CHx_Toggle(CHx)`
  flips it. These functions report whether the operation succeeded.
- `Dout_CHxs_PinState(mask)` sets all outputs from a bitmask.
  `Dout_Analysis(data, source)` handles the one-byte output command protocol:
  ASCII `1`-`8` toggle individual channels, `9` turns all on, and `0` turns all
  off.
- `Dout_Immediate()`, `Dout_Immediate_CHxs()`, and `Dout_Immediate_CHxn()`
  apply a state to one channel, a channel bitmask, or an array of channel
  states. `STATE_Retain` leaves a channel unchanged.

### Wi-Fi web interface

- `WIFI_Init()` starts the configured access point, sets up the HTTP routes,
  and starts `WebTask()`. `WebTask()` services incoming web requests.
- `handleRoot()` serves the output-control page. `handleGetData()` returns the
  current output states, and `handleSwitch(ledNumber)` handles a channel or
  all-output switch request.
- `parseData()`, `ParseRTCData()`, and `ParseRtcConfig()` parse web-submitted
  output schedules, serial schedules, and RTC settings.
  `ParseRS485Config()`, `ParseRS485Data()`, and
  `ParseRS485BaudRateConfig()` parse RS485 settings and requests.
  `ParseCANData()` and `ParseCANRateConfig()` parse CAN send and bitrate
  requests. These parsers return whether parsing succeeded.
- `handleGetRS485Data()` and `handleGetCANData()` serve buffered received
  data. `handleRS485Send()` and `handleCANSend()` send data through their
  respective interfaces.

### Bluetooth, RS485, and CAN

- `Bluetooth_Init()` starts the BLE service and advertising.
  `Bluetooth_SendData(data)` sends a notification to connected BLE clients.
  `BLETask()` handles BLE background work.
- `RS485_UpdateBaudRate(baud)` changes the RS485 serial speed.
  `SetData(data, length)` transmits bytes, and `ReadData(buffer, length)`
  reads bytes from the interface. `RS485_Loop()` processes incoming data;
  `RS485Task()` runs that work in the background. `RS485_Analysis(data)`
  handles the supported external relay commands when expansion support is
  enabled.
- `CAN_UpdateRate(rate)` requests a bitrate change.
  `CAN_Set_Bitrate(rate)` reconfigures the CAN driver and returns an ESP-IDF
  error code. `send_message(id, data, length, extended)` queues a CAN frame;
  `send_message_Bytes(message)` sends an existing TWAI message.
  `CAN_Loop()` and `CANTask()` process CAN activity.

### RTC and hardware support

- `PCF85063_Init()` initializes the RTC. `PCF85063_Set_Time()`,
  `PCF85063_Set_Date()`, and `PCF85063_Set_All()` set clock fields;
  `PCF85063_Read_Time()` reads them. `datetime_to_str()` formats a date/time.
- `TimerEvent_CHx_Set()`, `TimerEvent_CHxs_Set()`, and
  `TimerEvent_CHxn_Set()` schedule individual or grouped output changes.
  `TimerEvent_Serial_Set()` schedules a serial action.
  `TimerEvent_printf_ALL()` lists the stored events, and
  `TimerEvent_Del_Number()` removes an event by number. `RTCTask()` checks
  scheduled events.
- `I2C_Read()` and `I2C_Write()` access I2C device registers.
  `TCA9554PWR_Init()` configures the output expander;
  `Read_EXIO()` / `Read_EXIOS()` read its pins, and `Set_EXIO()` /
  `Set_EXIOS()` set individual pins or a bitmask.
- `RGB_Light()` sets the status LED color; `RGB_Open_Time()` schedules a
  timed or blinking indication. `Buzzer_Open()` / `Buzzer_Closs()` control the
  buzzer, and `Buzzer_Open_Time()` schedules it.
- `Flash_test()` checks flash storage. `File_Search()` checks whether a file
  exists, and `Folder_retrieval()` lists files matching an extension.
  `Acquisition_time()` obtains network time over Ethernet and sets the RTC.
