#include <Adafruit_NeoPixel.h>
//#include <NimBLEDevice.h>

//#define BLESERIAL_USE_NIMBLE true
#include <BLESerial.h>
//#include <Embedded_Template_Library.h>
//#include <etl/queue.h>
//#include <etl/circular_buffer.h>

BLESerial<> SerialBLE;
//BLESerial<etl::queue<uint8_t, 255, etl::memory_model::MEMORY_MODEL_SMALL>> SerialBLE;
//BLESerial<etl::circular_buffer<uint8_t, 255>> SerialBLE;

#define LED_PWR 17
#define LED_PIN 18
#define LED_COUNT 1

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);


#define PUMP_PIN A0


//BLESerial<etl::queue<uint8_t, 255, etl::memory_model::MEMORY_MODEL_SMALL>> SerialBLE;

void setLED(int r, int g, int b) {
    if ((r+g+b) == 0) {
      digitalWrite(LED_PWR, LOW);
    } else {
      digitalWrite(LED_PWR, HIGH);
    }
    pixel.setPixelColor(0, pixel.Color(r,g,b));
    pixel.show();
  }

void setup() {
    Serial.begin(9600);
    SerialBLE.begin("OIL-BOY");
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(LED_PWR, OUTPUT);      // Set the RGB_PWR pin to OUTPUT
   // digitalWrite(LED_PWR , HIGH);   // Turn on the RGB_PWR (LDO2)
    pixel.begin();
    pixel.setBrightness(50);  // Set brightness (0-255)
  }



void runPump(int duration) {
    Serial.print("Activating pump for ");
    Serial.print(duration);
    Serial.println(" ms");

    setLED(128, 0, 128);
   
    digitalWrite(PUMP_PIN, HIGH);  // Turn pump ON
    delay(duration);               // Wait for the defined duration
    digitalWrite(PUMP_PIN, LOW);   // Turn pump OFF

    setLED(0, 0, 0);

    Serial.println("Pump deactivated");
}
void processCommand(String str)
  {
  int msPump;

  if (str.startsWith("OIL:")) {
    msPump = str.substring(4).toInt();
    if (msPump != 0) { 
      runPump(msPump);
      Serial.println("OK");
      return;
      }
  } else {
    Serial.println("ERR");
    }
  }

String strCommand;

void processSerial() 
  {
    if (Serial.available()) {
        //int c;
       // c = Serial.read();

        SerialBLE.write(Serial.read());
        SerialBLE.flush();
    }
    if (SerialBLE.available()) {
        char c;
        c = SerialBLE.read();
        if (c == '\n') {
        
          processCommand(strCommand);
          strCommand = "";
        } else {
          strCommand.concat(c);
        }
        Serial.write(c);
    }  
  }


void loop()
  {
  processSerial();
  }