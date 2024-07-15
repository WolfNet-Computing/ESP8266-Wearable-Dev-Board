#include <Arduino.h>

#include "config.hpp"
#include "torch.hpp"

using namespace std;

Torch::Torch(uint8_t ledpin) : _ledpin(ledpin) {
  pinMode(_ledpin, OUTPUT);
  Torch::off();
}

void Torch::on(void) {
  digitalWrite(_ledpin, LED_ON);
}

void Torch::off(void) {
  digitalWrite(_ledpin, LED_OFF);
}