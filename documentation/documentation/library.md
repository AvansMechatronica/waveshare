# Library-referentie: waveshare_board

De library `libraries/waveshare_board` bevat de drivers voor alle hardware op het
board. Alle firmwareprojecten gebruiken deze library. Neem in je eigen code
gewoon de header op, bijvoorbeeld `#include "WS_Dout.h"`. PlatformIO vindt en
bouwt de library automatisch.

De meeste modules starten bij hun `*_Init()` een eigen FreeRTOS-taak. Na de
initialisatie hoef je alleen de functies aan te roepen; de taken doen het
periodieke werk.

| Module | Header | Functie |
| --- | --- | --- |
| [Digitale uitgangen](#digitale-uitgangen-ws-dout) | `WS_Dout.h` | 8 uitgangen via TCA9554 |
| [Digitale ingangen](#digitale-ingangen-ws-din) | `WS_DIN.h` | 8 ingangen |
| [RGB-led en zoemer](#rgb-led-en-zoemer-ws-gpio) | `WS_GPIO.h` | Statusindicatie |
| [RS485](#rs485-ws-rs485) | `WS_RS485.h` | Seriële bus, Modbus-uitbreiding |
| [CAN](#can-ws-can) | `WS_CAN.h` | CAN-bus via TWAI |
| [Real-time clock](#real-time-clock-ws-pcf85063) | `WS_PCF85063.h` | PCF85063 tijd en alarm |
| [Timer-events](#timer-events-ws-rtc) | `WS_RTC.h` | Tijdgestuurde acties |
| [Bluetooth LE](#bluetooth-le-ws-bluetooth) | `WS_Bluetooth.h` | Bediening via BLE |
| [Ethernet](#ethernet-ws-eth) | `WS_ETH.h` | W5500 en NTP-tijd |
| [SD-kaart](#sd-kaart-ws-sd) | `WS_SD.h` | Bestanden op micro-SD |
| [I/O-expander](#i-o-expander-ws-tca9554pwr) | `WS_TCA9554PWR.h` | Laag niveau TCA9554 |
| [I²C](#i2c-i2c-driver) | `I2C_Driver.h` | Laag niveau I²C |
| [Gedeelde typen](#gedeelde-typen-ws-struct-en-ws-information) | `WS_Struct.h`, `WS_Information.h` | Enums en opties |

---

## Digitale uitgangen (WS_Dout)

De 8 uitgangen worden aangestuurd via de TCA9554 I/O-expander. Kanaalnummers lopen
van 1 tot en met 8 (`GPIO_PIN_CH1` … `GPIO_PIN_CH8`).

| Functie | Beschrijving |
| --- | --- |
| `void Dout_Init(void)` | Initialiseert de expander (alle pinnen als uitgang) en start een bewakingstaak die bij een fout de led en zoemer laat knipperen. |
| `bool Dout_CHx(uint8_t CHx, bool State)` | Zet uitgang `CHx` (1–8) aan (`true`) of uit (`false`). Geeft `true` bij succes. |
| `bool Dout_Open(uint8_t CHx)` / `bool Dout_Closs(uint8_t CHx)` | Zet één uitgang aan of uit. |
| `bool Dout_CHx_Toggle(uint8_t CHx)` | Wisselt de toestand van één uitgang. |
| `bool Dout_CHxs_PinState(uint8_t PinState)` | Zet alle 8 uitgangen tegelijk; bit 0 = CH1 … bit 7 = CH8. |
| `void Dout_Immediate(uint8_t CHx, bool State, uint8_t Mode_Flag)` | Als `Dout_CHx`, maar werkt ook `Dout_Flag[]` bij en logt de bron (`Mode_Flag`, zie `Trigger_event`). |
| `void Dout_Immediate_CHxs(uint8_t PinState, uint8_t Mode_Flag)` | Zet alle uitgangen via een bitmasker en werkt `Dout_Flag[]` bij. |
| `void Dout_Immediate_CHxn(Status_adjustment *Dout_n, uint8_t Mode_Flag)` | Zet per kanaal `STATE_Open`, `STATE_Closs` of `STATE_Retain` (ongewijzigd laten). `Dout_n` is een array van 8 elementen. |
| `void Dout_Analysis(uint8_t *buf, uint8_t Mode_Flag)` | Interpreteert een commando-teken: `'1'`…`'8'` wisselt een kanaal, `'9'` zet alles aan, `'0'` alles uit. Wordt gebruikt door Bluetooth en RS485. |
| `bool Dout_Flag[8]` | Laatst bekende toestand van elke uitgang. |

```cpp
#include "WS_Dout.h"

Dout_Immediate(1, true, WIFI_Mode_Trigger);   // CH1 aan
Dout_Immediate_CHxs(0b10000001, WIFI_Mode_Trigger); // alleen CH1 en CH8 aan
```

## Digitale ingangen (WS_DIN)

| Functie / symbool | Beschrijving |
| --- | --- |
| `void DIN_Init(void)` | Zet de ingangspinnen (GPIO 4–11) als input met pull-up en start `DINTask`, die elke 20 ms de ingangen leest. |
| `bool DIN_Flag[8]` | Actuele toestand van de ingangen CH1–CH8. |
| `Dout_Immediate_Default` | `1` (standaard): een verandering op de ingangen schakelt direct de bijbehorende uitgangen. Zet op `0` in `WS_DIN.h` om dit uit te schakelen. |
| `DIN_Inverse_Enable` | `1`: uitgangen volgen de ingangen geïnverteerd. |

:::{note}
Zolang `Dout_Immediate_Default` op `1` staat, nemen de ingangen bij elke
verandering de uitgangen over. Wil je de uitgangen volledig vanuit je eigen code
sturen, zet het dan op `0`.
:::

## RGB-led en zoemer (WS_GPIO)

De led en zoemer werken met een wachtrij van maximaal 10 signalen, die door een
eigen taak worden afgespeeld. De functies blokkeren dus niet.

| Functie | Beschrijving |
| --- | --- |
| `void GPIO_Init()` | Initialiseert led en zoemer en start `RGBTask` en `BuzzerTask`. |
| `void RGB_Light(uint8_t r, uint8_t g, uint8_t b)` | Zet de led direct op een kleur (0–255 per kanaal). |
| `void RGB_Open_Time(uint8_t r, uint8_t g, uint8_t b, uint16_t Time, uint16_t flicker_time)` | Laat de led `Time` ms branden; met `flicker_time` > 50 ms knippert hij met die periode. |
| `void Buzzer_Open(void)` / `void Buzzer_Closs(void)` | Zoemer direct aan of uit. |
| `void Buzzer_Open_Time(uint16_t Time, uint16_t flicker_time)` | Zoemer `Time` ms aan, eventueel pulserend met `flicker_time` (> 50 ms). |
| `void Set_Dutyfactor(uint16_t dutyfactor)` | PWM-dutycycle van de zoemer (0–255), dus het volume. |

```cpp
RGB_Open_Time(0, 50, 0, 1000, 0);     // 1 s groen
RGB_Open_Time(50, 0, 0, 2000, 200);   // 2 s rood knipperen
Buzzer_Open_Time(500, 100);           // 0,5 s pulserend piepen
```

## RS485 (WS_RS485)

RS485 gebruikt UART1 (TX GPIO 17, RX GPIO 18). Standaard 9600 baud, 8N1.

| Functie / symbool | Beschrijving |
| --- | --- |
| `void RS485_Init()` | Start de seriële poort en `RS485Task`, die ontvangen data opslaat voor de webinterface. |
| `void SetData(uint8_t* data, size_t length)` | Verstuurt `length` bytes over RS485. |
| `void ReadData(uint8_t* buf, uint8_t length)` | Leest `length` bytes als die beschikbaar zijn. |
| `void RS485_UpdateBaudRate(unsigned long baud)` | Wijzigt de baudrate tijdens bedrijf. |
| `void RS485_Analysis(uint8_t *buf)` | Stuurt een externe Modbus RTU relaismodule aan (kanaal 1–8, `Extension_ALL_ON`, `Extension_ALL_OFF`). Actief als `Extension_Enable` = 1. |
| `unsigned long RS485_BaudRate` | Huidige baudrate. |

```cpp
uint8_t msg[] = {0x01, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x8C, 0x3A};
SetData(msg, sizeof(msg));
```

## CAN (WS_CAN)

CAN gebruikt de interne TWAI-controller (TX GPIO 2, RX GPIO 3). Standaard 250 kbit/s.

| Functie / symbool | Beschrijving |
| --- | --- |
| `void CAN_Init(void)` | Installeert en start de TWAI-driver en `CANTask`, die berichten ontvangt en busfouten afhandelt. |
| `void send_message(uint32_t CAN_ID, uint8_t* Data, uint8_t Data_length, bool Frame_type)` | Verstuurt een bericht. `Frame_type` = `true` voor een extended frame (29-bit ID), `false` voor standaard (11-bit). |
| `void send_message_Bytes(twai_message_t message)` | Verstuurt een volledig opgebouwd TWAI-bericht. |
| `esp_err_t CAN_Set_Bitrate(uint32_t bitrate_kbps)` / `void CAN_UpdateRate(uint32_t bitrate_kbps)` | Wijzigt de bitrate (bijv. 125, 250, 500, 1000). |
| `uint32_t CAN_bitrate_kbps` | Huidige bitrate. |

```cpp
uint8_t data[] = {0x11, 0x22, 0x33};
send_message(0x123, data, sizeof(data), false);  // standaard frame
```

## Real-time clock (WS_PCF85063)

De PCF85063 zit op de I²C-bus (adres `0x51`). Tijden worden doorgegeven als `datetime_t`:

```cpp
typedef struct {
  uint16_t year; uint8_t month; uint8_t day; uint8_t dotw;
  uint8_t hour; uint8_t minute; uint8_t second;
} datetime_t;
```

| Functie / symbool | Beschrijving |
| --- | --- |
| `void PCF85063_Init(void)` | Initialiseert de RTC en start een taak die `datetime` elke 100 ms bijwerkt. Wordt aangeroepen door `RTC_Init()`. |
| `datetime_t datetime` | Actuele datum en tijd. |
| `void PCF85063_Set_All(datetime_t time)` | Zet datum en tijd. |
| `void PCF85063_Set_Time(datetime_t time)` / `void PCF85063_Set_Date(datetime_t date)` | Zet alleen de tijd of alleen de datum. |
| `void PCF85063_Read_Time(datetime_t *time)` | Leest de tijd direct uit de RTC. |
| `void PCF85063_Set_Alarm(datetime_t time)` / `void PCF85063_Read_Alarm(datetime_t *time)` | Alarmtijd zetten of lezen. |
| `void PCF85063_Enable_Alarm(void)` / `uint8_t PCF85063_Get_Alarm_Flag()` | Alarm inschakelen en de alarmvlag uitlezen. |
| `void datetime_to_str(char *str, datetime_t time)` | Zet een tijd om naar tekst. |

## Timer-events (WS_RTC)

Met timer-events voer je acties uit op een vast tijdstip, eventueel herhaald.
Er kunnen maximaal 10 events tegelijk actief zijn.

| Functie | Beschrijving |
| --- | --- |
| `void RTC_Init(void)` | Initialiseert de RTC en start `RTCTask`, die de events uitvoert. |
| `void TimerEvent_CHx_Set(datetime_t time, uint8_t CHx, bool State, Repetition_event Repetition)` | Zet uitgang `CHx` op tijdstip `time` aan of uit. |
| `void TimerEvent_CHxs_Set(datetime_t time, uint8_t PinState, Repetition_event Repetition)` | Zet alle uitgangen volgens een bitmasker. |
| `void TimerEvent_CHxn_Set(datetime_t time, Status_adjustment *Dout_n, uint32_t *repetition_Time, Repetition_event Repetition)` | Zet per kanaal aan, uit of ongewijzigd, met een eigen herhaaltijd. |
| `void TimerEvent_Serial_Set(datetime_t time, Web_Receive* SerialData, Repetition_event Repetition)` | Verstuurt RS485- of CAN-data op een tijdstip. |
| `void TimerEvent_printf_ALL(void)` | Toont alle actieve events op de seriële monitor. |
| `void TimerEvent_Del_Number(uint8_t Event_Number)` | Verwijdert een event. |

`Repetition_event` bepaalt de herhaling: `Repetition_NONE`, `_Milliseconds`,
`_Seconds`, `_Minutes`, `_Hours`, `_Everyday`, `_Weekly` of `_Monthly`.

```cpp
datetime_t t = datetime;   // nu
t.hour = 7; t.minute = 30; t.second = 0;
TimerEvent_CHx_Set(t, 1, true, Repetition_Everyday);   // elke dag om 07:30 CH1 aan
```

## Bluetooth LE (WS_Bluetooth)

Het board adverteert als `ESP32-S3-POE-ETH-8DI-8DO` (NimBLE). Een client schrijft
commando's naar de RX-characteristic en ontvangt meldingen via TX.

| Functie / symbool | Beschrijving |
| --- | --- |
| `void Bluetooth_Init()` | Start de BLE-server en de taak die elke seconde het IP-adres (`ipStr`) naar een verbonden client stuurt. |
| `void Bluetooth_SendData(char *Data)` | Stuurt tekst als notificatie naar alle verbonden clients. |
| `SERVICE_UUID`, `RX_CHARACTERISTIC_UUID`, `TX_CHARACTERISTIC_UUID` | UUID's van de service en characteristics. |

Commando's via RX:

| Data | Actie |
| --- | --- |
| `0x31`…`0x38` | Uitgang 1–8 wisselen |
| `0x39` / `0x30` | Alle uitgangen aan / uit |
| `0x06` + kanaal | Externe Modbus RTU relaismodule via RS485 aansturen |
| `0x0A 0x0B` + data | Data over RS485 versturen |
| `0x0A 0x0C` + 4 bytes ID + 1 byte frametype + data | Data over CAN versturen |

:::{note}
`WS_Bluetooth` verwacht dat het project `char ipStr[16]` definieert. In alle
projecten gebeurt dat in `wifi_app.cpp`.
:::

## Ethernet (WS_ETH)

| Functie | Beschrijving |
| --- | --- |
| `void ETH_Init(void)` | Start de W5500 (SPI: SCK 15, MISO 14, MOSI 13, CS 16, IRQ 12, RST 39) en `EthernetTask`. Het IP-adres komt via DHCP. |
| `void Acquisition_time(void)` | Haalt de tijd op via NTP (`pool.ntp.org`) en zet de RTC. Gebeurt automatisch zodra Ethernet een IP-adres heeft. |

De tijdzone is ingesteld met `#define timezone 8` in `WS_ETH.h` (UTC+8). Pas deze
aan voor Nederland: `1` in de winter, `2` in de zomer.

## SD-kaart (WS_SD)

De kaart wordt in 1-bit SD_MMC-modus gebruikt (CLK 48, CMD 47, D0 45) en is
gekoppeld als `/sdcard`. Gebruik daarna de standaard `SD_MMC`-functies van Arduino.

| Functie / symbool | Beschrijving |
| --- | --- |
| `void SD_Init()` | Koppelt de SD-kaart en bepaalt de grootte. |
| `void Flash_test()` | Toont de flashgrootte op de seriële monitor. |
| `bool File_Search(const char* directory, const char* fileName)` | Controleert of een bestand bestaat. |
| `uint16_t Folder_retrieval(const char* directory, const char* fileExtension, char File_Name[][100], uint16_t maxFiles)` | Zoekt bestanden met een extensie en geeft het aantal terug. |
| `uint16_t SDCard_Size`, `uint16_t Flash_Size` | Grootte van SD-kaart en flash (MB). |

## I/O-expander (WS_TCA9554PWR)

Laag-niveau toegang tot de TCA9554 (I²C-adres `0x20`), waarop de uitgangen zitten.
Gebruik bij voorkeur de functies uit `WS_Dout`.

| Functie | Beschrijving |
| --- | --- |
| `void TCA9554PWR_Init(uint8_t PinMode = 0x00, uint8_t PinState = 0x00)` | Mode (0 = output, 1 = input) en begintoestand van alle pinnen. |
| `bool Set_EXIO(uint8_t Pin, uint8_t State)` / `bool Set_EXIOS(uint8_t PinState)` | Eén pin of alle pinnen zetten. |
| `bool Set_Toggle(uint8_t Pin)` | Pin wisselen. |
| `uint8_t Read_EXIO(uint8_t Pin)` / `uint8_t Read_EXIOS(uint8_t REG)` | Pin of register lezen (`TCA9554_OUTPUT_REG` voor de uitgangstoestand). |
| `void Mode_EXIO(uint8_t Pin, uint8_t State)` / `void Mode_EXIOS(uint8_t PinState)` | Pinmodus instellen. |
| `uint8_t Read_REG(uint8_t REG)` / `uint8_t Write_REG(uint8_t REG, uint8_t Data)` | Ruwe registertoegang. |

## I²C (I2C_Driver)

| Functie | Beschrijving |
| --- | --- |
| `void I2C_Init(void)` | Start de I²C-bus op SDA GPIO 42 en SCL GPIO 41. |
| `bool I2C_Read(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t len)` | Leest `len` bytes vanaf register `reg`. |
| `bool I2C_Write(uint8_t addr, uint8_t reg, const uint8_t *data, uint32_t len)` | Schrijft `len` bytes naar register `reg`. |

## Gedeelde typen (WS_Struct en WS_Information)

| Type / define | Beschrijving |
| --- | --- |
| `Trigger_event` | Bron van een actie: `DIN_Mode_Trigger`, `RS485_Mode_Trigger`, `Bluetooth_Mode_Trigger`, `WIFI_Mode_Trigger`, `RTC_Mode_Trigger`, `MQTT_Mode_Trigger`. Wordt gebruikt als `Mode_Flag`. |
| `Repetition_event` | Herhaling van timer-events (zie [Timer-events](#timer-events-ws-rtc)). |
| `Status_adjustment` | `STATE_Closs` (uit), `STATE_Open` (aan), `STATE_Retain` (ongewijzigd). |
| `Web_Receive` | Beschrijving van een RS485- of CAN-bericht voor timer-events. |
| `Extension_Enable` | `1` (standaard): RS485-commando's sturen een externe Modbus relaismodule aan. Overschrijfbaar met `-DExtension_Enable=0` in `platformio.ini`. |
