#include <Arduino.h>

#include "config.hpp"
#include "button.hpp"

using namespace std;

Button::Button() {
  pinMode(BUTTON_C, INPUT_PULLUP);
}

// THIS FUNCTION IS BLOCKING...
// Will probably end up using timers instead of this method in the final version...
bool Button::isPressed(int button) {
  if ((button == BUTTON_C))
  {
    int _last_c_value = digitalRead(button);
    delay(DEBOUNCE_DELAY);

    int _button_c = digitalRead(button);
    if (_button_c != _last_c_value)
    {
      return false;
    }

    _last_c_value = _button_c;
    if (_button_c == LOW)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}