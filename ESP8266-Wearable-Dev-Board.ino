// https://arduino-esp8266.readthedocs.io/en/latest/ for more on esp8266 programming using the Arduino IDE.

#include "Config.hpp"
#include "DisplayUI.hpp"
#include "Torch.hpp"

#define DEBUG true

// These defines must be put before #include <ESP_MultiResetDetector.h>
// to select where to store MultiResetDetector's variable.
// For ESP32, You must select one to be true. (EEPROM or SPIFFS)
// For ESP8266, You must select one to be true. (RTC, EEPROM, LITTLEFS or SPIFFS)
// Otherwise, library will use default EEPROM storage.

#ifdef ESP8266
  #define USE_LITTLEFS false
  #define ESP_MRD_USE_LITTLEFS true
  #define ESP_MRD_USE_SPIFFS false
  #define ESP_MRD_USE_EEPROM false
#else
  #error This code is intended to run on the ESP8266 or ESP32 platform! Please check your setting under Tools -> Board.
#endif

#include <ESP_WiFiManager.h>  //https://github.com/khoih-prog/ESP_WiFiManager
#define MRD_TIMES 3
#define MRD_TIMEOUT 1
#define MRD_ADDRESS 0
#define MULTIRESETDETECTOR_DEBUG false
#include <ESP_MultiResetDetector.h>  //https://github.com/khoih-prog/ESP_MultiResetDetector
MultiResetDetector* mrd;

bool firstBoot = false;
bool altBoot = false;

#include <ESP8266WiFi.h>  //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#define USE_LITTLEFS true

#if USE_LITTLEFS
#include <LittleFS.h>
FS* filesystem = &LittleFS;
#define FileFS LittleFS
#define FS_Name "LittleFS"
#else
FS* filesystem = &SPIFFS;
#define FileFS SPIFFS
#define FS_Name "SPIFFS"
#endif

#define ESP_getChipId() (ESP.getChipId())

// You only need to format the filesystem once, but if you need to, then set this value to true...
#define FORMAT_FILESYSTEM false

#define MIN_AP_PASSWORD_SIZE 8

#define SSID_MAX_LEN 32
// WPA2 passwords can be up to 63 characters long...
#define PASS_MAX_LEN 64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw[PASS_MAX_LEN];
} WiFi_Credentials;

typedef struct
{
  String wifi_ssid;
  String wifi_pw;
} WiFi_Credentials_String;

#define NUM_WIFI_CREDENTIALS 10

typedef struct
{
  WiFi_Credentials WiFi_Creds[NUM_WIFI_CREDENTIALS];
} WM_Config;

WM_Config WM_config;

#define CONFIG_FILENAME F("/wifi_cred.dat")

#include <ESPAsync_WiFiManager.h> // https://github.com/khoih-prog/ESPAsync_WiFiManager
#include <PubSubClient.h>         // https://pubsubclient.knolleary.net/

// SSID and PW for Config Portal
String ssid = CONFIG_SSID;
String password = CONFIG_PASS;

DisplayUI displayUI;

Torch torch;
simplebutton::Button* resetButton;

wl_status_t wifiStatus;
wl_status_t wifiStatusPrev;

bool booted = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);

  #if ESP_MRD_USE_LITTLEFS
    Serial.println("using LittleFS");
  #elif ESP_MRD_USE_SPIFFS
    Serial.println("using SPIFFS");
  #elif ESP_MRD_USE_RTC
    Serial.println("using RTC");
  #else
    Serial.println("using EEPROM");
  #endif

  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
  if (mrd->detectMultiReset()) {
    Serial.println("Multi Reset Detected");
    altBoot = true;
  } else {
    Serial.println("No Multi Reset Detected");
    altBoot = false;
  }

  // Get time... (Will eventually be from a NTP server!)
  currentTime = millis();

  if ((WiFi.SSID() == "") && !(altBoot)) {
#if AUTHOR
#include "auth.hpp"
#else
    Serial.println("No AP credentials");
    firstBoot = true;
#endif
  }

  if (firstBoot)
  {
    Serial.println("Starting Config Portal");
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
    // Start display
    //if (settings::getDisplaySettings().enabled) {
        displayUI.setup();
        displayUI.mode = DISPLAY_MODE::ALTINTRO;
    //}
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
  else
  {
    // Start display
    //if (settings::getDisplaySettings().enabled) {
        displayUI.setup();
        displayUI.mode = DISPLAY_MODE::INTRO;
    //}
  }

  // (En/Dis)able serial command interface...
  //if (settings::getCLISettings().enabled) {
  //  cli.enable();
  //} else {
  //  prntln(SETUP_SERIAL_WARNING);
  //  Serial.flush();
  //  Serial.end();
  //}
  //cli.load();

#if defined(TORCH_PIN)
  torch.setup();
#endif

  // Setup reset button...
  resetButton = new ButtonPullup(RESET_BUTTON);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Simple display of WiFi status...
  wifiStatus = WiFi.status();
  if ((wifiStatus != WL_CONNECTED) && (wifiStatus != wifiStatusPrev))
  {
    displayUI.clear();
    displayUI.drawString(0, "Failed to connect");
    displayUI.refresh();
    wifiStatusPrev = wifiStatus;
  }
  else if (wifiStatus != wifiStatusPrev)
  {
    displayUI.clear();
    displayUI.drawString(0, "Local IP:");
    displayUI.drawString(1, String(WiFi.localIP()));
    displayUI.refresh();
    wifiStatusPrev = wifiStatus;
  }

  // Call the multi reset detector loop method every so often,
  // so that it can recognise when the timeout expires.
  // Calling mrd.stop() will no longer consider the next reset a multi reset.
  mrd->loop();
}
