# Aan de slag

## Benodigdheden

- Een Waveshare ESP32-S3-POE-ETH-8DI-8DO board en een USB-C datakabel.
- [Visual Studio Code](https://code.visualstudio.com/) met de
  [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)-extensie,
  of [PlatformIO Core](https://platformio.org/install/cli).
- Toegang tot een WiFi-netwerk en/of Ethernet, afhankelijk van de gekozen firmware.

## Repository clonen

De broncode staat op GitHub:
[AvansMechatronica/waveshare](https://github.com/AvansMechatronica/waveshare).

::::{tab-set}

:::{tab-item} Git (command line)
```bash
git clone https://github.com/AvansMechatronica/waveshare.git
cd waveshare
```
:::

:::{tab-item} VS Code
1. Open het Command Palette (`Ctrl+Shift+P`) en kies **Git: Clone**.
2. Plak `https://github.com/AvansMechatronica/waveshare.git`.
3. Kies een map en open daarna de gekloonde repository.
:::

:::{tab-item} Zonder Git
Download de code als ZIP via
[Code → Download ZIP](https://github.com/AvansMechatronica/waveshare/archive/refs/heads/main.zip)
en pak het bestand uit.
:::

::::

:::{note}
Clone altijd de volledige repository, en niet één projectmap. De projecten
gebruiken de gedeelde map `libraries/`, die naast de projectmappen moet staan.
:::

Bijwerken naar de laatste versie:

```bash
git pull
```

## Workspace openen

Open `waveshare.code-workspace` in VS Code. Alle firmwareprojecten verschijnen dan
als aparte mappen. PlatformIO herkent elk project aan zijn eigen `platformio.ini`.

## Bouwen, uploaden en monitoren

Voer de commando's uit in de map van het project:

::::{tab-set}

:::{tab-item} Template (AP of STA)
```bash
cd main_wifi_template
pio run -e waveshare_esp32s3_sta              # bouwen
pio run -e waveshare_esp32s3_sta -t upload    # uploaden
pio device monitor -e waveshare_esp32s3_sta   # seriële monitor
```
Vervang `waveshare_esp32s3_sta` door `waveshare_esp32s3_ap` voor access-point mode.
:::

:::{tab-item} Overige projecten
```bash
cd main_wifi_sta
pio run -e waveshare_esp32s3_io
pio run -e waveshare_esp32s3_io -t upload
pio device monitor -e waveshare_esp32s3_io
```
:::

::::

De seriële monitor draait op 115200 baud via de ingebouwde USB-C-poort (native CDC).

:::{note}
Als `pio` niet in het pad staat, gebruik dan het volledige pad, bijvoorbeeld
`%USERPROFILE%\.platformio\penv\Scripts\pio.exe` op Windows, of gebruik de
knoppen in de PlatformIO-statusbalk van VS Code.
:::

## Board-instellingen in platformio.ini

Alle projecten gebruiken dezelfde basisinstellingen voor de N16R8-module:

| Instelling | Waarde | Waarom |
| --- | --- | --- |
| `board` | `esp32-s3-devkitc-1` | Generiek ESP32-S3 board als basis |
| `board_build.arduino.memory_type` | `qio_opi` | Octal PSRAM van de N16R8 |
| `board_build.partitions` | `default_16MB.csv` | Volledige 16 MB flash gebruiken |
| `-DBOARD_HAS_PSRAM` | build flag | PSRAM activeren |
| `-DARDUINO_USB_CDC_ON_BOOT=1` | build flag | Seriële output via USB-C |
| `lib_extra_dirs` | `../libraries` | Gedeelde libraries, waaronder `waveshare_board` |

## Afhankelijkheden

Via `lib_deps` worden deze libraries automatisch geïnstalleerd:

- `arduino-libraries/Ethernet`
- `bblanchon/ArduinoJson`
- `h2zero/NimBLE-Arduino`: vereist voor de Bluetooth-code. De arduino-esp32 3.x
  core bevat geen klassieke Bluedroid BLE-library (`BLEDevice.h`) meer.

`NTPClient` en `PubSubClient` staan lokaal in `libraries/`.
