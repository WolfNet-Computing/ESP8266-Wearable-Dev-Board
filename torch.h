#ifndef TORCH_H
#define TORCH_H 1
#pragma once

#include <Arduino.h>

using namespace std;

// For ESP8266
#ifdef ESP8266
  #define TORCH_ON                        LOW
  #define TORCH_OFF                       HIGH
#endif

class Torch {
  private:
    uint8_t _ledpin;

  public:
    Torch(uint8_t pin);
    void on(void);
    void off(void);
};

#endif