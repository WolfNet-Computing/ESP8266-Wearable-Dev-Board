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

#define DEBUG                     true

#ifdef ESP8266
  #define USE_LITTLEFS            false
  #define ESP_MRD_USE_LITTLEFS    true
  #define ESP_MRD_USE_SPIFFS      false
  #define ESP_MRD_USE_EEPROM      false
  #define ESP_MRD_USE_RTC         false
#else
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your Tools->Board setting.
#endif
#include <ESP_WiFiManager.h>                    //https://github.com/khoih-prog/ESP_WiFiManager
#define MRD_TIMES                     3
#define MRD_TIMEOUT                   10
#define MRD_ADDRESS                   0
#define MULTIRESETDETECTOR_DEBUG      false
#include <ESP_MultiResetDetector.h>            //https://github.com/khoih-prog/ESP_MultiResetDetector
MultiResetDetector* mrd;

bool altBoot = false;
bool configPortal = false;

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#define USE_LITTLEFS      true

#if USE_LITTLEFS
  #include <LittleFS.h>
  FS* filesystem =      &LittleFS;
  #define FileFS        LittleFS
  #define FS_Name       "LittleFS"
#else
  FS* filesystem =      &SPIFFS;
  #define FileFS        SPIFFS
  #define FS_Name       "SPIFFS"
#endif
  
#define ESP_getChipId()   (ESP.getChipId())

// You only need to format the filesystem once
//#define FORMAT_FILESYSTEM       true
#define FORMAT_FILESYSTEM         false

#define MIN_AP_PASSWORD_SIZE    8

#define SSID_MAX_LEN            32
//WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN            64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

typedef struct
{
  String wifi_ssid;
  String wifi_pw;
}  WiFi_Credentials_String;

#define NUM_WIFI_CREDENTIALS      2

typedef struct
{
  WiFi_Credentials  WiFi_Creds [NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config         WM_config;

#define  CONFIG_FILENAME              F("/wifi_cred.dat")

#include <ESP_WiFiManager.h>              //https://github.com/khoih-prog/ESP_WiFiManager

// SSID and PW for Config Portal
String ssid = "ESP_" + String(ESP_getChipId(), HEX);
String password = "your_password";

Torch torch(TORCH_PIN);

wl_status_t wifiStatus;
wl_status_t wifiStatusPrev;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.print("Starting ESP_MultiResetDetector minimal on ");
  Serial.println(ARDUINO_BOARD);
#if ESP_MRD_USE_LITTLEFS
  Serial.println("using LittleFS");
#elif ESP_MRD_USE_SPIFFS
  Serial.println("using SPIFFS");
#elif ESP_MRD_USE_RTC
  Serial.println("using RTC");
#else
  Serial.println("using EEPROM");
#endif
  Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);

  display.init();

  display.normalDisplay();
#if FLIP_DISPLAY
  display.flipScreenVertically();
#endif
  // Available default fonts: ArialMT_Plain_10, ArialMT_Plain_16,
  // ArialMT_Plain_24.
  // Or create one with the font tool at http://oleddisplay.squix.ch
  display.setFont(ArialMT_Plain_10);
  display.setColor(WHITE);
  display.displayOn();
  display.clear();
  display.drawXbm(0, 0, 128, 64, startup_logo);
  display.display();
  delay(2000);

  display.clear();
  display.drawString(0, 0, "ESP8266 Wearable");
  display.drawString(0, 20, "Dev Board ~Alpha~");
  display.display();
  delay(2000);

  if (WiFi.SSID() == "")
  {
    #if AUTHOR
      #include "auth.h"
    #else
      Serial.println("No AP credentials");
      configPortal = true;
    #endif
  }

  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
  if (mrd->detectMultiReset()) {
    Serial.println("Multi Reset Detected");
    //digitalWrite(LED_PIN, LOW);
    altBoot = true;
  }
  else {
    Serial.println("No Multi Reset Detected");
    //digitalWrite(LED_PIN, HIGH);
  }

  if (configPortal)
  {
    Serial.println("Starting Config Portal");
    //digitalWrite(LED_PIN, LOW);
    ESP_WiFiManager ESP_wifiManager("Dev-Board");
    ESP_wifiManager.setConfigPortalTimeout(0);

    if (!ESP_wifiManager.startConfigPortal())
    {
      Serial.println("Not connected to WiFi");
    }
    else
    {
      Serial.println("connected");
    }
  }

  if (altBoot)
  {
    Serial.println("Alternate boot mode!");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to connect");
  }
  else
  {
    Serial.println("Local IP: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  wifiStatus = WiFi.status();
  if ((wifiStatus != WL_CONNECTED) && (wifiStatus != wifiStatusPrev))
  {
    display.cls();
    display.println("Failed to connect");
    display.display();
    wifiStatusPrev = wifiStatus;
  }
  else if (wifiStatus != wifiStatusPrev)
  {
    display.cls();
    display.println("Local IP: ");
    display.println(WiFi.localIP());
    display.display();
    wifiStatusPrev = wifiStatus;
  }
  delay(20);
  // Call the multi reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // You can also call mrd.stop() when you wish to no longer
  // consider the next reset as a multi reset.
  mrd->loop();
}
