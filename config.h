#ifndef CONFIG_H
#define CONFIG_H	1
#pragma once

#if defined(DSTIKE_DEAUTHER_WATCH)

// ===== TORCH ===== //
  #define LED_NEOPIXEL_GRB
  #define LED_NUM 1
  #define LED_NEOPIXEL_PIN 15

  #define TORCH_PIN 16

// ===== DISPLAY ===== //
  #define SH1106_I2C
  #define FLIP_DIPLAY true

// ===== BUTTONS ===== //
  #define BUTTON_UP 13
  #define BUTTON_DOWN 12
  #define BUTTON_A 14
  #define BUTTON_B 2
  #define BUTTON_C 5
#endif

#endif