#pragma once

// Shared board options. Project-specific settings (WiFi, MQTT, ...) live in each project.
// Override with a build flag in platformio.ini, e.g. -DExtension_Enable=0
#ifndef Extension_Enable
#define Extension_Enable      1                   // Whether to extend the connection to external devices     1:Expansion device Modbus RTU Dout     0:No extend
#endif
