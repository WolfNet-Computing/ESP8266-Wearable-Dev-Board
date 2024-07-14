#pragma once

#include <Wire.h>

#if DISPLAY_SCREEN == sh1106
  #include "SH1106Wire.h"

  // By default SH1106Wire set I2C frequency to 700000, you can use set either another frequency or skip setting the frequency by providing -1 value
  SH1106Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL
#endif
/* May need an 'else' clause here for other screen types... */