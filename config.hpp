#ifndef CONFIG_H
#define CONFIG_H	1
#pragma once

/* Comment out this variable if you have a standard board. */
#define RIPOFF true

#if defined(DSTIKE_DEAUTHER_WATCH) && RIPOFF
  #define LED_NEOPIXEL_GRB
  #define LED_PIN             1
  #define LED_NEOPIXEL_PIN    15

  #define LED_ON              LOW
  #define LED_OFF             HIGH

  #define TORCH_PIN           16

  #define SH1106_I2C
  #define FLIP_DISPLAY         true

  typedef struct Buttons {
    uint8_t up = 13;
    uint8_t down = 12;
    uint8_t a = 14;
    uint8_t b = 0;
    uint8_t c = 2;
  };
#endif

#endif