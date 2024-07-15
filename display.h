#ifndef DISPLAY_H
#define DISPLAY_H 1
#pragma once

#ifdef SH1106_I2C
  #include <Wire.h>
  #include "SH1106Wire.h"
  SH1106Wire display = SH1106Wire(0x3C, 5, 4);
#endif
/* May need an 'else' clause here for other screen types... */

#endif