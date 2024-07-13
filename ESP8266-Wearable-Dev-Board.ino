// These defines must be put before #include <ESP_MultiResetDetector.h>
// to select where to store MultiResetDetector's variable.
// For ESP32, You must select one to be true (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true (RTC, EEPROM, LITTLEFS or SPIFFS)
// Otherwise, library will use default EEPROM storage

#define ESP8266_MRD_USE_RTC             false   //true

#define ESP_MRD_USE_LITTLEFS            true
#define ESP_MRD_USE_SPIFFS              false
#define ESP_MRD_USE_EEPROM              false

#define MULTIRESETDETECTOR_DEBUG        false  //false

#include <ConfigAssist.h>
#include <ConfigAssistHelper.h>

#include <ESPTelnet.h>

#include <ESP_MultiResetDetector.h>

#include <Wire.h>
#include "SH1106Wire.h"

MultiResetDetector* mrd;

// For ESP8266
#define LED_ON                          LOW
#define LED_OFF                         HIGH

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
    
  Serial.begin(115200);
  while (!Serial) {
    delay(200);
    Serial.print(F("\nStarting ESP_MultiResetDetector minimal on ")); Serial.print(ARDUINO_BOARD);

  #if ESP_MRD_USE_LITTLEFS
    Serial.println(F(" using LittleFS"));
  #elif ESP_MRD_USE_SPIFFS
    Serial.println(F(" using SPIFFS"));
  #else
    Serial.println(F(" using EEPROM"));
  #endif

    Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
    
    mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);

    if (mrd->detectMultiReset()) 
    {
      Serial.println("Multi Reset Detected");
      digitalWrite(LED_BUILTIN, LED_ON);
    } 
    else 
    {
      Serial.println("No Multi Reset Detected");
      digitalWrite(LED_BUILTIN, LED_OFF);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // Call the multi reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call mrd.stop() when you wish to no longer
  // consider the next reset as a multi reset.
  mrd->loop();
}
