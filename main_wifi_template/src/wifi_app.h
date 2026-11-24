#pragma once

#include "stdio.h"
#include <stdint.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "WS_GPIO.h"
#include "WS_Information.h"

/*
 * WiFi configuration comes from platformio.ini (build_flags per environment).
 * Exactly one of APP_WIFI_MODE_AP / APP_WIFI_MODE_STA must be defined.
 */
#if defined(APP_WIFI_MODE_AP) && defined(APP_WIFI_MODE_STA)
  #error "Define only one of APP_WIFI_MODE_AP or APP_WIFI_MODE_STA in platformio.ini"
#elif !defined(APP_WIFI_MODE_AP) && !defined(APP_WIFI_MODE_STA)
  #error "Define APP_WIFI_MODE_AP or APP_WIFI_MODE_STA in platformio.ini"
#endif

#ifndef APP_WIFI_SSID
  #error "Define APP_WIFI_SSID in platformio.ini"
#endif
#ifndef APP_WIFI_PASSWORD
  #define APP_WIFI_PASSWORD ""
#endif

#ifdef APP_WIFI_MODE_AP
  #ifndef APP_WIFI_AP_IP
    #define APP_WIFI_AP_IP 192,168,4,1
  #endif
  #ifndef APP_WIFI_AP_SUBNET
    #define APP_WIFI_AP_SUBNET 255,255,255,0
  #endif
  #ifndef APP_WIFI_AP_CHANNEL
    #define APP_WIFI_AP_CHANNEL 1
  #endif
  #ifndef APP_WIFI_AP_MAX_CLIENTS
    #define APP_WIFI_AP_MAX_CLIENTS 4
  #endif
#endif

#ifdef APP_WIFI_MODE_STA
  #ifndef APP_WIFI_HOSTNAME
    #define APP_WIFI_HOSTNAME "esp32s3-8di-8do"
  #endif
#endif

extern char ipStr[16];              // Current IP address as text (also sent via Bluetooth)
extern bool WIFI_Connection;        // AP: true once the AP is up. STA: true while connected
extern WebServer server;            // Register extra pages with server.on() before WIFI_Init()

void handleRoot();
void WIFI_Init();
void WifiTask(void *parameter);
