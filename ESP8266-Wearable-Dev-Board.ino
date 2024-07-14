// These defines must be put before #include <ESP_MultiResetDetector.h>
// to select where to store MultiResetDetector's variable.
// For ESP32, You must select one to be true. (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true. (RTC, EEPROM, LITTLEFS or SPIFFS)
// Otherwise, library will use default EEPROM storage.

// https://arduino-esp8266.readthedocs.io/en/latest/ for more on esp8266 programming using the Arduino IDE.

#include "config.h"
#include "display.h"
#include "torch.h"
#include "startup_logo.h"

#define ESP8266_MRD_USE_RTC             false

#define ESP_MRD_USE_LITTLEFS            true
#define ESP_MRD_USE_SPIFFS              false
#define ESP_MRD_USE_EEPROM              false

#define MULTIRESETDETECTOR_DEBUG        false

#include <ESP_MultiResetDetector.h>
Torch torch(TORCH_PIN);
MultiResetDetector* mrd;

void setup() {
  // put your setup code here, to run once:
  display.init();
  display.clear();
  display.drawXbm(0, 0, 128, 64, startup_logo); // assuming your bitmap is 128x64
  display.display();

  // Sets the current font. Available default fonts
  // ArialMT_Plain_10, ArialMT_Plain_16, ArialMT_Plain_24
  // Or create one with the font tool at http://oleddisplay.squix.ch
  display.setFont(ArialMT_Plain_10);

  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
  if (mrd->detectMultiReset()) 
  {
    display.println("Multi Reset Detected");
    torch.on();
  } 
  else 
  {
    display.println("No Multi Reset Detected");
    torch.off();
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
