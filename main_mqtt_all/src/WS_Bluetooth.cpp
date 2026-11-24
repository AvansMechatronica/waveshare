#include "WS_Bluetooth.h"
/*
Data           : Control digital outputs (data specifies the digital channel: 0x31 = channel 1, 0x32 = channel 2, 0x39 = all low, 0x30 = all high)
06 + data      : Control a relay channel on an external Modbus RTU Relay via RS485 (data specifies the relay channel: 0x01 = relay 1, 0x02 = relay 2, 0x39 = all relays on, 0x3A = all relays off)
0A 0B + data   : Send data over RS485 (0A 0C + multiple data bytes; the following data is sent)
0A 0C + data   : Send data over CAN (0A 0C + 4-byte ID + 1-byte frame type (0x01 = extended frame, 0x00 = standard frame) + multiple data bytes; the following data is sent after configuring CAN parameters as specified above)
*/
BLEServer* pServer;                                                             // Used to represent a BLE server
BLECharacteristic* pTxCharacteristic;
BLECharacteristic* pRxCharacteristic;

/**********************************************************  Bluetooth   *********************************************************/

class MyServerCallbacks : public BLEServerCallbacks {                           //By overriding the onConnect() and onDisconnect() functions
    void onConnect(BLEServer* pServer) {                                        // When the Device is connected, "Device connected" is printed.
    Serial.println("Device connected"); 
  }

  void onDisconnect(BLEServer* pServer) {                                       // "Device disconnected" will be printed when the device is disconnected
    Serial.println("Device disconnected");

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                 // Re-broadcast so that the device can query
    pAdvertising->addServiceUUID(SERVICE_UUID);                                 // Re-broadcast so that the device can query
    pAdvertising->setScanResponse(true);                                        // Re-broadcast so that the device can query
    pAdvertising->setMinPreferred(0x06);                                        // Re-broadcast so that the device can query 
    pAdvertising->setMinPreferred(0x12);                                        // Re-broadcast so that the device can query 
    BLEDevice::startAdvertising();                                              // Re-broadcast so that the device can query 
    pRxCharacteristic->notify();                                                // Re-broadcast so that the device can query  
    pAdvertising->start();                                                      // Re-broadcast so that the device can query
  }
};
class MyRXCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {                            // The onWrite function is called when the remote device sends data to your feature
    String rxValue = pCharacteristic->getValue();
    if (!rxValue.isEmpty()) {
      // The received data rxValue is processed here
      uint16_t Ble_Length = rxValue.length();
      if(Ble_Length == 1)
      {
        printf("%s\n", rxValue.c_str());                                        // Print output through the serial port
        uint8_t* valueBytes = reinterpret_cast<uint8_t*>(const_cast<char*>(rxValue.c_str())); // Convert value to uint8 t*
        Dout_Analysis(valueBytes,Bluetooth_Mode_Trigger);                              // pilot dout
      }
      else if(Ble_Length == 2)
      {
        if(Extension_Enable)
        {
          printf("%s\n", rxValue.c_str());                                      // Print output through the serial port       
          uint8_t* valueBytes = reinterpret_cast<uint8_t*>(const_cast<char*>(rxValue.c_str())); // Convert value to uint8 t*
          if(valueBytes[0] == 0x06)                                             // Instruction check correct
            RS485_Analysis(valueBytes);                                         // Control external dout
          else
            printf("Note : Non-instruction data was received - Bluetooth !\r\n");
        }
        else
          printf("Note : Non-instruction data was received or external douts are not enabled - Bluetooth !\r\n");
      }
      else if(Ble_Length > 2)
      {
        printf("%s\n", rxValue.c_str());                                      // Print output through the serial port       
        uint8_t* valueBytes = reinterpret_cast<uint8_t*>(const_cast<char*>(rxValue.c_str())); // Convert value to uint8 t*
        if(valueBytes[0] == 0x0A && valueBytes[1] == 0x0B)                    // Instruction check correct
          SetData(valueBytes + 2, Ble_Length - 2);                            // Send data from the RS485
        else if(valueBytes[0] == 0x0A && valueBytes[1] == 0x0C && Ble_Length > 7){
            twai_message_t Ble_message;
            Ble_message.identifier = (valueBytes[2] << 24) |(valueBytes[3] << 16) | (valueBytes[4] << 8) |(valueBytes[5]) ;
            Ble_message.extd = valueBytes[6];                                 // Frame_type : 1：Extended frames   0：Standard frames
            Ble_message.data_length_code = Ble_Length - 7;                    // valueBytes[0] to valueBytes[6] are configuration parameters
            for (int i = 0; i < Ble_message.data_length_code; i++) {
              Ble_message.data[i] = valueBytes[i + 7];
            }
            send_message_Bytes(Ble_message);                                  // Send data from the CAN
        }
        else
          printf("Note : Non-instruction data was received - Bluetooth !\r\n");
      }
      else
      {
        printf("Note : Non-instruction data was received - Bluetooth !\r\n");
      }
      pRxCharacteristic->setValue("");                                           // After data is read, set it to blank for next read
    }
  }
};
void Bluetooth_SendData(char* Data) {  // Send data using Bluetooth
  if (Data != nullptr && strlen(Data) > 0) {
    if (pServer->getConnectedCount() > 0) {
      String SendValue = String(Data);  // Convert char* to String
      pTxCharacteristic->setValue(SendValue);  // Set SendValue to the eigenvalue (String type)
      pTxCharacteristic->notify();  // Sends a notification to all connected devices
    }
  }
}
void Bluetooth_Init()
{
  /*************************************************************************
  Bluetooth
  *************************************************************************/
  BLEDevice::init("ESP32-S3-POE-ETH-8DI-8DO");                                        // Initialize Bluetooth and start broadcasting                           
  pServer = BLEDevice::createServer();                                          
  pServer->setCallbacks(new MyServerCallbacks());                               
  BLEService* pService = pServer->createService(SERVICE_UUID);                  
  pTxCharacteristic = pService->createCharacteristic(
                                    TX_CHARACTERISTIC_UUID,
                                    BLECharacteristic:: PROPERTY_READ);         // The eigenvalues are readable and can be read by remote devices
  pRxCharacteristic = pService->createCharacteristic(
                                    RX_CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_WRITE);         // The eigenvalues are writable and can be written to by remote devices
  pRxCharacteristic->setCallbacks(new MyRXCallback());

  pRxCharacteristic->setValue("Successfully Connect To ESP32-S3-POE-ETH-8DI-8DO");      
  pService->start();   

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();                   
  pAdvertising->addServiceUUID(SERVICE_UUID);                                   
  pAdvertising->setScanResponse(true);                                          
  pAdvertising->setMinPreferred(0x06);                                          
  pAdvertising->setMinPreferred(0x12);                                          
  BLEDevice::startAdvertising();                                                
  pRxCharacteristic->notify();                                                    
  pAdvertising->start();
  RGB_Open_Time(0, 0, 60,1000, 0); 
  printf("Now you can read it in your phone!\r\n");
  xTaskCreatePinnedToCore(
    BLETask,    
    "BLETask",   
    4096,                
    NULL,                 
    2,                   
    NULL,                 
    0                   
  );
}

void BLETask(void *parameter) {
  while(1){
    Bluetooth_SendData(ipStr);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  vTaskDelete(NULL);
}