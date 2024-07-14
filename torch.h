#ifndef TORCH_H
#define TORCH_H 1
#pragma once

#include <Arduino.h>

using namespace std;

class Torch {
  private:
    uint8_t _ledpin;

  public:
    Torch(uint8_t pin);
    void on(void);
    void off(void);
};

#endif