#ifndef CONFIG_H
#define CONFIG_H	1
#pragma once

/* Uncomment this variable if you have a modified board. */
#define RIPOFF 1

#if defined(DSTIKE_DEAUTHER_WATCH) && (RIPOFF)

// ===== TORCH ===== //
  #define LED_NEOPIXEL_GRB
  #define LED_PIN             1
  #define LED_NEOPIXEL_PIN    15
  #define LED_ON              LOW
  #define LED_OFF             HIGH

  #define TORCH_PIN           16

// ===== DISPLAY ===== //
  #define SH1106_I2C
  #define FLIP_DIPLAY         true

// ===== BUTTONS ===== //
  #define BUTTON_UP           13
  #define BUTTON_DOWN         12
  #define BUTTON_A            14
  #define BUTTON_B            0
  #define BUTTON_C            2
#endif

#endif