#include <Adafruit_NeoPixel.h>

/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#include <Adafruit_NeoPixel.h>
#define LED_PWR 17
#define LED_PIN 18
#define LED_COUNT 1

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


#define PUMP_PIN A0
int pumpRunTime = 1235;


void activatePump(int duration) {
    Serial.print("Activating pump for ");
    Serial.print(duration);
    Serial.println(" ms");

    pixel.setPixelColor(0, pixel.Color(128,0,128));
    pixel.show();

    digitalWrite(PUMP_PIN, HIGH);  // Turn pump ON
    delay(duration);               // Wait for the defined duration
    digitalWrite(PUMP_PIN, LOW);   // Turn pump OFF

    pixel.setPixelColor(0, pixel.Color(0,0,0));
    pixel.show();

    Serial.println("Pump deactivated");
}


class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      
      pumpRunTime = atoi(value.c_str());
      Serial.print("Pump Run Time: ");
      Serial.println(value);
      
    }
  }
};

void setup() {
  Serial.begin(115200);

  Serial.println("OilBoy");
 
  BLEDevice::init("ESP_Oilboy");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LED_PWR, OUTPUT);      // Set the RGB_PWR pin to OUTPUT
  digitalWrite(LED_PWR , HIGH);   // Turn on the RGB_PWR (LDO2)
  pixel.begin();
  pixel.show();
  pixel.setBrightness(50);  // Set brightness (0-255)
}

void loop() {
  if (pumpRunTime != 0){
    activatePump(pumpRunTime);
    pumpRunTime = 0;
  }
  delay(100);
}