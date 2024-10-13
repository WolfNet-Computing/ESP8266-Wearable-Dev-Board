#ifndef CONFIG_H
#define CONFIG_H	1
#pragma once

/* Comment out this variable if you have a standard board. */
#define RIPOFF true

/* These next couple of #DEFINEs set the default SSID and password for the configuration portal. */
#define CONFIG_SSID "ESP8266_Dev-Board"
#define CONFIG_PASS "Default_123"

#if defined(DSTIKE_DEAUTHER_WATCH) && RIPOFF
  #define LED_NEOPIXEL_GRB
  #define LED_PIN             1
  #define LED_NEOPIXEL_PIN    15

  #define LED_ON              LOW
  #define LED_OFF             HIGH

  #define TORCH_PIN           16

  #define SH1106_I2C
  #define FLIP_DISPLAY         true

  #define BUTTON_UP 13
  #define BUTTON_DOWN 12
  #define BUTTON_A 14
  #define BUTTON_B 0
  #define BUTTON_C 2
#endif

#endif