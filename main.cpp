// #include <Arduino.h>
// /*
//   Rui Santos & Sara Santos - Random Nerd Tutorials
//   Complete project details at https://RandomNerdTutorials.com/esp32-web-bluetooth/
//   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
//   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// */
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>

// BLEServer* pServer = NULL;
// BLECharacteristic* pSensorCharacteristic = NULL;
// BLECharacteristic* pLedCharacteristic = NULL;
// bool deviceConnected = false;
// bool oldDeviceConnected = false;
// uint32_t value = 0;

// const int ledPin = 2; // Use the appropriate GPIO pin for your setup

// // See the following for generating UUIDs:
// // https://www.uuidgenerator.net/
// #define SERVICE_UUID        "9b23de8e-d759-4327-9fd0-50935931ea09"
// #define SENSOR_CHARACTERISTIC_UUID "c4fc1687-6e0c-4e59-ab8a-815d91096ed3"
// #define LED_CHARACTERISTIC_UUID "a98b92ca-2453-461a-907f-9efb35623d91"

// class MyServerCallbacks: public BLEServerCallbacks {
//   void onConnect(BLEServer* pServer) {
//     deviceConnected = true;
//   };

//   void onDisconnect(BLEServer* pServer) {
//     deviceConnected = false;
//   }
// };

// class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
//   void onWrite(BLECharacteristic* pLedCharacteristic) {
//     std::string ledvalue  = pLedCharacteristic->getValue(); 
//     String value = String(ledvalue.c_str());
//     if (value.length() > 0) {
//       Serial.print("Characteristic event, written: ");
//       Serial.println(static_cast<int>(value[0])); // Print the integer value

//       int receivedValue = static_cast<int>(value[0]);
//       if (receivedValue == 1) {
//         digitalWrite(ledPin, HIGH);
//       } else {
//         digitalWrite(ledPin, LOW);
//       }
//     }
//   }
// };

// void setup() {
//   Serial.begin(115200);
//   pinMode(ledPin, OUTPUT);

//   // Create the BLE Device
//   BLEDevice::init("ESP32_Jorrit");

//   // Create the BLE Server
//   pServer = BLEDevice::createServer();
//   pServer->setCallbacks(new MyServerCallbacks());

//   // Create the BLE Service
//   BLEService *pService = pServer->createService(SERVICE_UUID);

//   // Create a BLE Characteristic
//   pSensorCharacteristic = pService->createCharacteristic(
//                       SENSOR_CHARACTERISTIC_UUID,
//                       BLECharacteristic::PROPERTY_READ   |
//                       BLECharacteristic::PROPERTY_WRITE  |
//                       BLECharacteristic::PROPERTY_NOTIFY |
//                       BLECharacteristic::PROPERTY_INDICATE
//                     );

//   // Create the ON button Characteristic
//   pLedCharacteristic = pService->createCharacteristic(
//                       LED_CHARACTERISTIC_UUID,
//                       BLECharacteristic::PROPERTY_WRITE
//                     );

//   // Register the callback for the ON button characteristic
//   pLedCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

//   // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
//   // Create a BLE Descriptor
//   pSensorCharacteristic->addDescriptor(new BLE2902());
//   pLedCharacteristic->addDescriptor(new BLE2902());

//   // Start the service
//   pService->start();

//   // Start advertising
//   BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
//   pAdvertising->addServiceUUID(SERVICE_UUID);
//   pAdvertising->setScanResponse(false);
//   pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
//   BLEDevice::startAdvertising();
//   Serial.println("Waiting a client connection to notify...");
// }

// void loop() {
//   // notify changed value
//   if (deviceConnected) {
//     pSensorCharacteristic->setValue(String(value).c_str());
//     pSensorCharacteristic->notify();
//     value++;
//     Serial.print("New value notified: ");
//     Serial.println(value);
//     delay(3000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
//   }
//   // disconnecting
//   if (!deviceConnected && oldDeviceConnected) {
//     Serial.println("Device disconnected.");
//     delay(500); // give the bluetooth stack the chance to get things ready
//     pServer->startAdvertising(); // restart advertising
//     Serial.println("Start advertising");
//     oldDeviceConnected = deviceConnected;
//   }
//   // connecting
//   if (deviceConnected && !oldDeviceConnected) {
//     // do stuff here on connecting
//     oldDeviceConnected = deviceConnected;
//     Serial.println("Device Connected");
//   }
// }

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Ultrasoon sensor pins
const int trigPin = D2; // D2
const int echoPin = D3; // D3

long duration;
float distance;

// BLE variabelen
BLEServer* pServer = NULL;
BLECharacteristic* pSensorCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

uint32_t value = 0;

// Zie de volgende link voor het genereren van UUID's: https://www.uuidgenerator.net/
#define SERVICE_UUID "9b23de8e-d759-4327-9fd0-50935931ea09"
#define SENSOR_CHARACTERISTIC_UUID "c4fc1687-6e0c-4e59-ab8a-815d91096ed3"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);
  
  // Ultrasone sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialiseer BLE
  BLEDevice::init("ESP32_Ultrasone_Sensor");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Maak de BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Maak een BLE Characteristic voor de sensor
  pSensorCharacteristic = pService->createCharacteristic(
      SENSOR_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  // Voeg een BLE Descriptor toe
  pSensorCharacteristic->addDescriptor(new BLE2902());

  // Start de service
  pService->start();

  // Start met adverteren
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // adverteer dit parameter niet
  BLEDevice::startAdvertising();
  Serial.println("Wacht op een clientverbinding om te notificeren...");
}

void loop() {
  // Ultrasone meting
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Afstand gemeten: ");
  Serial.println(distance);

  // Notify changed value
  if (deviceConnected) {
    String afstandString = String(distance);
    pSensorCharacteristic->setValue(afstandString.c_str());
    pSensorCharacteristic->notify();
    Serial.print("Verzonden via BLE: ");
    Serial.println(afstandString);
    delay(1000); // Wacht 1 seconde voordat de volgende waarde wordt verzonden
  }

  // Verbreek verbinding en start opnieuw met adverteren indien nodig
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // geef de BLE stack tijd om zich te herstellen
    pServer->startAdvertising(); // start opnieuw met adverteren
    Serial.println("Start opnieuw met adverteren");
    oldDeviceConnected = deviceConnected;
  }

  // Herstellen van verbinding
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}
