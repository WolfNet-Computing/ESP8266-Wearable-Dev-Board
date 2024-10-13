#ifndef BUTTON_H
#define BUTTON_H 1
#pragma once

#include <Arduino.h>

using namespace std;

#define DEBOUNCE_DELAY  200

class Button {
  public:
    Button();
    bool isPressed(int);
};

#endif