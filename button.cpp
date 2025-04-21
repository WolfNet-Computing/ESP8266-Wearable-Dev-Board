#include <Arduino.h>

#include "config.hpp"
#include "button.hpp"

using namespace std;

Button::Button() {
  pinMode(BUTTON_B, INPUT_PULLUP);
}