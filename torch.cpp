#include <Arduino.h>

#include "Config.hpp"
#include "Torch.hpp"

using namespace std;

void Torch::setup(void) {
  pinMode(TORCH_PIN, OUTPUT);
  Torch::off();
  _torchState = false;
}

void Torch::on(void) {
  digitalWrite(TORCH_PIN, LED_ON);
  _torchState = true;
}

void Torch::off(void) {
  digitalWrite(TORCH_PIN, LED_OFF);
  _torchState = false;
}

void Torch::toggle(void) {
  if (_torchState)
  {
    off();
  }
  else
  {
    on();
  }
}