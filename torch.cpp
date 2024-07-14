#include <Arduino.h>
#include "torch.h"

using namespace std;

Torch::Torch(uint8_t ledpin) : _ledpin(ledpin) {
  pinMode(_ledpin, OUTPUT);
  }

void Torch::on(void) {
  digitalWrite(_ledpin, TORCH_ON);
}

void Torch::off(void) {
  digitalWrite(_ledpin, TORCH_OFF);
}