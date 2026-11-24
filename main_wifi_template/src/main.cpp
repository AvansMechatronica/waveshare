#include <HardwareSerial.h>     // Reference the ESP32 built-in serial port library
#include "wifi_app.h"
#include "WS_Bluetooth.h"
#include "WS_GPIO.h"
#include "WS_RS485.h"
#include "WS_CAN.h"
#include "WS_RTC.h"
#include "WS_DIN.h"
#include "WS_SD.h"
#include "WS_ETH.h"
#include "application_tasks.h"


uint32_t Simulated_time=0;      // Analog time counting

/********************************************************  Initializing  ********************************************************/
void setup() {
  Flash_test();
  GPIO_Init();  // RGB . Buzzer GPIO
  I2C_Init();
  RTC_Init();// RTC
  RS485_Init();
  CAN_Init();
  SD_Init();
  WIFI_Init();// WIFI (AP or STA, selected in platformio.ini)
  Bluetooth_Init();// Bluetooth
  ETH_Init();

  DIN_Init();                 // If you don't want to control the dout through DIN, change Dout_Immediate_Default to 0 in WS_DIN.h and re-burn the program
  Dout_Init();

#ifdef APP_WIFI_MODE_AP
  printf("Connect to the WIFI network named \"%s\" and open the AP IP address printed above in a browser\r\n", APP_WIFI_SSID);
#else
  printf("Joining the WIFI network \"%s\"; the web page is served on the IP address printed after connecting\r\n", APP_WIFI_SSID);
#endif
  StartApplicationTasks();
}

/**********************************************************  While  **********************************************************/
void loop() {

}
