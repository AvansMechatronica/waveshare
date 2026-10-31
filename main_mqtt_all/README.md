# Waveshare ESP32-S3 MQTT I/O Firmware

PlatformIO firmware for the Waveshare ESP32-S3-POE-ETH-8DI-8DO. It brings up
the board's digital I/O and communication interfaces, provides a local web
interface over Wi-Fi, and connects to an MQTT broker.

## Features

- Read eight digital inputs and control eight digital outputs.
- Control outputs from the web interface, MQTT, Bluetooth LE, and scheduled
  RTC events.
- Configure and use RS485 and CAN.
- Connect using Wi-Fi and W5500 Ethernet.
- Initialize the onboard RTC and access SD card and flash storage.

## Hardware and software

- Waveshare ESP32-S3-POE-ETH-8DI-8DO board with an ESP32-S3-WROOM-1U N16R8
  module and W5500 Ethernet.
- [PlatformIO Core](https://platformio.org/install/cli) or the PlatformIO IDE
  extension for VS Code.
- A USB connection to the board for programming and serial monitoring.

The project is configured for the `waveshare_esp32s3_io` PlatformIO
environment. PlatformIO installs the dependencies declared in
`platformio.ini`.

## Configure

1. Open `src/WS_Information.h`.
2. Set the Wi-Fi network name and password, and configure the MQTT broker,
   client ID, and publish/subscribe topics for your deployment.
3. Do not commit real Wi-Fi passwords or MQTT credentials. Use test credentials
   for development, and rotate any credentials that have already been exposed.

## Build, upload, and monitor

Run these commands from this project directory:

```sh
pio run -e waveshare_esp32s3_io
pio run -e waveshare_esp32s3_io -t upload
pio device monitor -e waveshare_esp32s3_io
```

The serial monitor is configured for **115200 baud**. After boot, the firmware
connects to the configured Wi-Fi network and prints its IP address. Open that
address in a browser on the same network to access the web interface.

## Firmware flow

`setup()` in `src/MAIN_MQTT_ALL.cpp` initializes flash diagnostics, status
GPIO, I2C, the RTC, RS485, CAN, SD storage, Wi-Fi, MQTT, Bluetooth LE,
Ethernet, digital inputs, and digital outputs. Most communication and
monitoring modules start their own FreeRTOS task during initialization.
`loop()` is currently empty; ongoing work is handled by those tasks.

## Function guide

The function names below are implemented in the corresponding `src` module.
Most are also declared in that module's header.

### Digital inputs and outputs

**`WS_DIN.cpp`**

- `DIN_Init()` configures all eight input pins as `INPUT_PULLUP`, reads their
  initial state, and starts `DINTask()`.
- `DIN_Read_CH1()` through `DIN_Read_CH8()` sample one input, update its
  cached flag and bit in the input-state byte, and return the sampled level.
- `DIN_Read_CHxs()` samples all eight inputs and returns their combined
  bitmask (CH1 is bit 0, CH8 is bit 7).
- `DINTask()` polls every 20 ms. With `Dout_Immediate_Default` enabled, a
  changed input bitmask is sent to the corresponding output channels. Set this
  option in `WS_DIN.h` to `0` to disable this input-to-output behavior.

**`WS_Dout.cpp`**

- `Dout_Init()` initializes the TCA9554PWR output expander and starts
  `DoutFailTask()` to report failed output operations using the RGB indicator
  and buzzer.
- `Dout_Open(CHx)`, `Dout_Closs(CHx)`, and `Dout_CHx(CHx, state)` set one
  output high, low, or to an explicit state. `Dout_CHx_Toggle(CHx)` toggles
  one output; these functions return whether the expander operation succeeded.
- `Dout_CHxs_PinState(mask)` writes all eight outputs from a bitmask.
- `Dout_Analysis(data, source)` processes the single-byte command protocol:
  ASCII `1`-`8` toggle a channel, `9` sets all outputs high, and `0` sets all
  outputs low. The source identifies the triggering interface.
- `Dout_Immediate()`, `Dout_Immediate_CHxs()`, and
  `Dout_Immediate_CHxn()` set one channel, all channels from a bitmask, or
  channels from an array of `Status_adjustment` values (`STATE_Retain` leaves
  a channel unchanged).

**`WS_TCA9554PWR.cpp`**

- `TCA9554PWR_Init(mode, state)` configures the expander's pin directions and
  initial output state.
- `Read_REG()` and `Write_REG()` access a device register. `Read_EXIO()` and
  `Read_EXIOS()` read one pin or a register containing the pin states.
- `Mode_EXIO()` / `Mode_EXIOS()` set the direction of one pin or the pin mask.
  `Set_EXIO()` / `Set_EXIOS()` write one output or the full output bitmask;
  `Set_Toggle()` toggles a pin. Pin numbers are 1-based.

### Wi-Fi and web interface

**`WS_WIFI.cpp`**

- `WIFI_Init()` starts `WifiStaTask()`. The task connects in Wi-Fi station mode
  using the credentials in `WS_Information.h`, retries after connection
  failures, prints the assigned IP, and serves the web UI while connected.
- `handleRoot()` serves the main output-control page. `handleGetData()` returns
  current output data. `handleSwitch()` and the `handleSwitch1()` through
  `handleSwitch8()` wrappers process individual output actions.
- `handleRTCDout()` and `handleRTCSerial()` serve the scheduled-event pages.
  Their related handlers create, list, and delete RTC events.
- `handleSerial()` serves the RS485/CAN page. `handleGetRS485Data()`,
  `handleRS485Send()`, `handleGetCANData()`, and `handleCANSend()` retrieve
  buffered data and send interface data.
- `parseData()`, `ParseRTCData()`, and `ParseRtcConfig()` parse output
  schedules, serial schedules, and RTC configuration from web requests.
  `ParseRS485Config()`, `ParseRS485Data()`, `ParseRS485BaudRateConfig()`,
  `ParseCANData()`, and `ParseCANRateConfig()` parse communication settings
  and send requests. These functions return `true` when parsing succeeds.

The web server listens on port 80. The firmware runs as a station on the
configured Wi-Fi network; connect a browser on the same network to the IP
printed on the serial monitor.

### MQTT

**`WS_MQTT.cpp`**

- `MQTT_Init()` starts `MQTTTask()`. The task waits for Wi-Fi connectivity,
  configures the broker and callback, reconnects when necessary, and services
  the MQTT client.
- `reconnect()` connects with the configured client ID and subscribes to the
  configured topic.
- `callback(topic, payload, length)` handles received JSON output commands in
  the form `{"data":{"CH1":1}}`. Channel keys are `CH1` through `CH8` and
  `ALL`; values `1` and `0` request high and low states.
- `sendJsonData()` adds the configured device ID to the outgoing JSON document,
  serializes it, and publishes it to the configured publish topic.

Broker address, port, client ID, and topic names are set in
`WS_Information.h`.

### RS485 and CAN

**`WS_RS485.cpp`**

- `RS485_Init()` starts UART1 in half-duplex RS485 mode at 9600 baud and starts
  `RS485Task()`.
- `RS485_UpdateBaudRate(baud)` changes the serial speed.
- `SetData(data, length)` transmits bytes. `ReadData(buffer, length)` reads a
  requested number of available bytes.
- `RS485_Loop()` polls incoming data, recognizes the board's predefined
  eight-byte output toggle commands, and appends received data to the
  RS485-read buffer with a timestamp.
- `RS485_Analysis(data)` sends predefined Modbus RTU relay commands to an
  external output expansion device.

**`WS_CAN.cpp`**

- `CAN_Init()` installs and starts the ESP32 TWAI driver at 250 kbps with an
  accept-all filter, enables receive/error alerts, and starts `CANTask()`.
- `CAN_UpdateRate(rate)` requests a bitrate change through `CAN_Set_Bitrate()`.
  Supported rates are 25, 50, 100, 125, 250, 500, 800, and 1000 kbps.
- `CAN_Set_Bitrate(rate)` stops and reinstalls the driver at a supported rate
  and returns an ESP-IDF error code if the change fails.
- `send_message(id, data, length, extended)` queues CAN frames for
  transmission. Payloads longer than eight bytes are split into multiple
  frames. `send_message_Bytes(message)` is a convenience wrapper for an
  existing TWAI message.
- `CANTask()` processes received frames and CAN alerts; `CAN_Loop()` is the
  polling entry point.

### Clock and scheduled events

**`WS_PCF85063.cpp` and `WS_RTC.cpp`**

- `PCF85063_Init()` initializes the RTC device. `PCF85063_Set_Time()`,
  `PCF85063_Set_Date()`, and `PCF85063_Set_All()` write clock fields;
  `PCF85063_Read_Time()` reads them into a `datetime_t`.
- `PCF85063_Set_Alarm()`, `PCF85063_Read_Alarm()`,
  `PCF85063_Enable_Alarm()`, and `PCF85063_Get_Alarm_Flag()` manage the RTC
  alarm. `PCF85063_Reset()` resets the device, and `datetime_to_str()` formats
  a date/time value.
- `RTC_Init()` initializes the clock and starts `RTCTask()` and
  `Continuous_Task()`. These tasks check configured schedules and trigger
  events.
- `TimerEvent_CHx_Set()`, `TimerEvent_CHxs_Set()`, and
  `TimerEvent_CHxn_Set()` schedule one output, an output bitmask, or per-channel
  output actions. `TimerEvent_Serial_Set()` schedules RS485 or CAN data.
- `TimerEvent_handling(event)` executes an event. `TimerEvent_printf()` and
  `TimerEvent_printf_ALL()` report configured events;
  `TimerEvent_Del()` and `TimerEvent_Del_Number()` remove events.
- Calendar schedules support one-time, daily, weekly, and monthly repetitions.
  Interval schedules are checked by `Continuous_Task()`.

When Ethernet obtains an IP address, `Acquisition_time()` fetches network time
using NTP and sets the onboard RTC.

### Board utilities and storage

**`WS_GPIO.cpp`**

- `GPIO_Init()` initializes the board status indicators and buzzer.
- `RGB_Light(red, green, blue)` sets the RGB indicator immediately.
  `RGB_Open_Time(red, green, blue, duration, flicker)` schedules a timed or
  flashing indication.
- `Buzzer_Open()` and `Buzzer_Closs()` turn the buzzer on and off.
  `Buzzer_Open_Time(duration, flicker)` schedules a buzzer indication.
  `Set_Dutyfactor(value)` adjusts buzzer PWM duty cycle.
- `RGBTask()` and `BuzzerTask()` process queued indicator patterns.

**`I2C_Driver.cpp`**

- `I2C_Init()` initializes I2C on the configured SCL/SDA pins.
- `I2C_Read(device, register, buffer, length)` and
  `I2C_Write(device, register, data, length)` perform register-based I2C
  transfers and return success status.

**`WS_ETH.cpp`**

- `ETH_Init()` initializes the W5500 over SPI, registers network-event
  handling, and starts `EthernetTask()`.
- `EthernetTask()` monitors Ethernet link/IP state and requests time
  synchronization after a connection.
- `Acquisition_time()` obtains NTP time and sets the PCF85063 RTC.
- `testClient(host, port)` is a simple TCP connectivity/HTTP diagnostic
  helper.

**`WS_SD.cpp`**

- `SD_Init()` configures the SD_MMC pins, mounts the card, and reports type and
  storage usage.
- `File_Search(directory, name)` returns whether a named entry exists in the
  specified directory.
- `Folder_retrieval(directory, extension, names, limit)` writes matching
  filenames into the supplied array and returns the number found (up to
  `limit`).
- `Flash_test()` reports the detected flash size; it is called during startup.

**`WS_Bluetooth.cpp`**

- `Bluetooth_Init()` starts the BLE service and its `BLETask()`.
- `Bluetooth_SendData(data)` sends a string over BLE.
- `BLETask()` handles Bluetooth control traffic, including output commands
  and scheduled RTC actions.

## Project layout

- `src/MAIN_MQTT_ALL.cpp` initializes the firmware modules.
- `src/WS_*.cpp` and `src/WS_*.h` implement the board features and interfaces.
- `src/WS_Information.h` contains network and MQTT configuration.
- `include/` is for project-wide headers.
- `lib/` is for project-specific libraries.
- `test/` is the PlatformIO test directory.
- `platformio.ini` defines the board environment, build options, and
  dependencies.
