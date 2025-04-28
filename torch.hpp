#ifndef TORCH_H
  #define TORCH_H 1
  #pragma once

  #include <Arduino.h>

  using namespace std;

  class Torch {
    private:
    bool  _torchState;
    public:
      void setup(void);
      void on(void);
      void off(void);
      void toggle(void);
  };
#endif