# Shared libraries

This directory contains Arduino libraries shared by the firmware projects.
Each project's `platformio.ini` adds this directory with `lib_extra_dirs`.
PlatformIO can then use these local copies when building the projects.

| Library | Version | Used by | Purpose |
| --- | --- | --- | --- |
| [ArduinoJson](ArduinoJson/) | 7.2.1 | `main_mqtt_all` | JSON serialization and deserialization for MQTT messages. |
| [NTPClient](NTPClient/) | 3.2.1 | All firmware projects | Retrieves network time from an NTP server. |
| [PubSubClient](PubSubClient/) | 2.8 | `main_mqtt_all` | MQTT client for publishing and receiving messages. |

The versions are recorded in each library's `library.properties`. These
libraries are also maintained upstream:

- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
- [NTPClient](https://github.com/arduino-libraries/NTPClient)
- [PubSubClient](https://github.com/knolleary/pubsubclient)

The firmware projects declare additional PlatformIO dependencies in their
individual `platformio.ini` files. Keep those declarations and the shared
library directory in mind when changing or updating a library.
