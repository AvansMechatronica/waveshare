# Veelgestelde vragen (FAQ)

**Het board start steeds opnieuw op (bootloop).**
: Controleer of `board_build.partitions = default_16MB.csv` en de PSRAM-instellingen
  (`qio_opi`, `-DBOARD_HAS_PSRAM`) in `platformio.ini` staan. Zonder deze instellingen
  start de N16R8-module niet goed op.

**Ik zie geen output in de seriële monitor.**
: Gebruik 115200 baud en de USB-C-poort. `-DARDUINO_USB_CDC_ON_BOOT=1` moet in de
  build flags staan.

**De build faalt met `BLEDevice.h: No such file or directory`.**
: De arduino-esp32 3.x core bevat alleen nog NimBLE. Voeg
  `h2zero/NimBLE-Arduino @ ^2.5.2` toe aan `lib_deps`.

**De build vindt `WS_GPIO.h` (of een andere `WS_*`-header) niet.**
: Controleer of `lib_extra_dirs = ../libraries` in `platformio.ini` staat en of het
  project naast de map `libraries/` in de workspace staat.

**Hoe kies ik tussen AP en STA in het template?**
: Zet `default_envs` in `platformio.ini` op `waveshare_esp32s3_ap` of
  `waveshare_esp32s3_sta`, of kies het environment in de PlatformIO-statusbalk.
  Zie [Template-project](template.md).

**De uitgangen volgen steeds de ingangen, ook als ik ze zelf aanstuur.**
: Zet `Dout_Immediate_Default` op `0` in `libraries/waveshare_board/src/WS_DIN.h`.
  Let op: dit geldt dan voor alle projecten.

**De tijd van de RTC klopt niet.**
: De NTP-tijd wordt alleen opgehaald via Ethernet. Pas de tijdzone aan in `WS_ETH.h`
  (`#define timezone`), standaard staat die op UTC+8.

**Welke IDE gebruik ik voor de documentatie?**
: Visual Studio Code werkt goed: je kunt de pagina's lokaal bouwen en bekijken.
  Zie [Documentatie bijwerken en publiceren](pages.md).
