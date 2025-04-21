#include <Arduino.h>

#include "Config.hpp"
#include "DisplayUI.hpp"
#include "StartupLogo.hpp"

#include "not-my-work/functions/functions.h"

void DisplayUI::configInit() {
  // initialize display
  display.init();
  /*
    In case of a compiler (conversion char/uint8_t) error,
    make sure to have version 4 of the display library installed
    https://github.com/ThingPulse/esp8266-oled-ssd1306/releases/tag/4.0.0

    Available default fonts: ArialMT_Plain_10, ArialMT_Plain_16,
    ArialMT_Plain_24.
    Or create one with the font tool at http://oleddisplay.squix.ch
  */
  display.setFont(ArialMT_Plain_10);
  display.setContrast(255);
  if (FLIP_DISPLAY) {display.flipScreenVertically();}
  display.setColor(WHITE);
  display.clear();
  display.display();
}

void DisplayUI::configOn() {
  display.displayOn();
}

void DisplayUI::configOff() {
  display.displayOff();
}

void DisplayUI::clear() {
  display.clear();
}

void DisplayUI::refresh() {
  display.display();
}

void DisplayUI::updatePrefix() {
  clear();
}

void DisplayUI::updateSuffix() {
  refresh();
}

void DisplayUI::drawString(int x, int y, String str) {
  display.drawString(x, y, replaceUtf8(str, String(QUESTIONMARK)));
}

void DisplayUI::drawString(int row, String str) {
  drawString(0, row * lineHeight, str);
}

void DisplayUI::drawLine(int x1, int y1, int x2, int y2) {
  display.drawLine(x1, y1, x2, y2);
}

void DisplayUI::drawBitmap(int pos_x, int pos_y, int size_x, int size_y, const unsigned char bitmap[]) {
  display.drawXbm(pos_x, pos_y, size_x, size_y, bitmap);
}

#ifdef HIGHLIGHT_LED
  void DisplayUI::setupLED() {
    pinMode(HIGHLIGHT_LED, OUTPUT);
    digitalWrite(HIGHLIGHT_LED, HIGH);
    torchLED = true;
  }
#endif // ifdef HIGHLIGHT_LED

void DisplayUI::setupButtons() {
  up   = new ButtonPullup(BUTTON_UP);
  down = new ButtonPullup(BUTTON_DOWN);
  a    = new ButtonPullup(BUTTON_A);
  b    = new ButtonPullup(BUTTON_B);

  // === BUTTON UP === //
  up->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;

    if (!tempOff) {
      if (mode == DISPLAY_MODE::MENU) {                 // when in menu, go up or down with cursor
        if (currentMenu->selected > 0) currentMenu->selected--;
        else currentMenu->selected = currentMenu->list->size() - 1;
      } else if (mode == DISPLAY_MODE::CLOCK) {         // when in clock, change time
        setTime(clockHour, clockMinute + 1, clockSecond);
      }
    }
  });

  up->setOnHolding([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;
    if (!tempOff) {
      if (mode == DISPLAY_MODE::MENU) {                 // when in menu, go up or down with cursor
        if (currentMenu->selected > 0) currentMenu->selected--;
        else currentMenu->selected = currentMenu->list->size() - 1;
      } else if (mode == DISPLAY_MODE::CLOCK) {         // when in clock, change time
        setTime(clockHour, clockMinute + 10, clockSecond);
      }
    }
  }, buttonDelay);

  // === BUTTON DOWN === //
  down->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;
    if (!tempOff) {
      if (mode == DISPLAY_MODE::MENU) {                 // when in menu, go up or down with cursor
        if (currentMenu->selected < currentMenu->list->size() - 1) currentMenu->selected++;
        else currentMenu->selected = 0;
      } else if (mode == DISPLAY_MODE::CLOCK) {         // when in packet monitor, change channel
        setTime(clockHour, clockMinute - 1, clockSecond);
      }
    }
  });

  down->setOnHolding([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;
    if (!tempOff) {
      if (mode == DISPLAY_MODE::MENU) {                 // when in menu, go up or down with cursor
        if (currentMenu->selected < currentMenu->list->size() - 1) currentMenu->selected++;
        else currentMenu->selected = 0;
      } else if (mode == DISPLAY_MODE::CLOCK) { // when in packet monitor, change channel
        setTime(clockHour, clockMinute - 10, clockSecond);
      }
    }
  }, buttonDelay);

  // === BUTTON A === //
  a->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;
    if (!tempOff) {
      switch (mode) {
        case DISPLAY_MODE::MENU:
          if (currentMenu->list->get(currentMenu->selected).click) {
            currentMenu->list->get(currentMenu->selected).click();
          }
          break;

        case DISPLAY_MODE::CLOCK:
        case DISPLAY_MODE::CLOCK_DISPLAY:
          mode = DISPLAY_MODE::MENU;
          display.setFont(DejaVu_Sans_Mono_12);
          display.setTextAlignment(TEXT_ALIGN_LEFT);
          break;
      }
    }
  });

  a->setOnHolding([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;
    if (!tempOff) {
      if (mode == DISPLAY_MODE::MENU) {
        if (currentMenu->list->get(currentMenu->selected).hold) {
          currentMenu->list->get(currentMenu->selected).hold();
        }
      }
    }
  }, 800);

  // === BUTTON B === //
  b->setOnClicked([this]() {
    scrollCounter = 0;
    scrollTime    = currentTime;
    buttonTime    = currentTime;
    highlightLED = !highlightLED;
    digitalWrite(TORCH_PIN, highlightLED);
  });
}

void DisplayUI::drawIntro() {
  drawBitmap(0, 0, 128, 64, startup_logo);
  updateSuffix();   // Display command...
  delay(3000);
  clear();
  drawString(0, center("ESP8266 Wearable", maxLen));
  drawString(1, "Dev Board  ~ Alpha ~");
  updateSuffix();   // Display command...
  delay(3000);
}

void DisplayUI::drawAltIntro() {
  clear();
  display.println("Update Mode...");
  updateSuffix();   // Display command...
  delay(3000);
}

void DisplayUI::setup() {
  configInit();
  setupButtons();
  buttonTime = currentTime;

  #ifdef RTC_DS3231
    bool h12;
    bool PM_time;
    clock.setClockMode(false);
    clockHour   = clock.getHour(h12, PM_time);
    clockMinute = clock.getMinute();
  #else // ifdef RTC_DS3231
    clockHour   = random(12);
    clockMinute = random(60);
  #endif // ifdef RTC_DS3231
}

void DisplayUI::clearMenu(Menu* menu) {
  while (menu->list->size() > 0) {
    menu->list->remove(0);
  }
}

void DisplayUI::changeMenu(Menu* menu) {
  if (menu) {
    /*
    // only open list menu if it has nodes
    if (((menu == &apListMenu) && (accesspoints.count() == 0)) ||
      ((menu == &stationListMenu) && (stations.count() == 0)) ||
      ((menu == &nameListMenu) && (names.count() == 0))) {
      return;
    }
    */

    if (currentMenu) clearMenu(currentMenu);
    currentMenu = menu;
    currentMenu->selected = 0;
    buttonTime = currentTime;

    /*
    if (selectedID < 0) selectedID = 0;
    */

    if (currentMenu->parentMenu) {
      addMenuNode(currentMenu, D_BACK, currentMenu->parentMenu); // add [BACK]
      currentMenu->selected = 1;
    }

    if (currentMenu->build) currentMenu->build();
  }
}

void DisplayUI::goBack() {
  if (currentMenu->parentMenu) changeMenu(currentMenu->parentMenu);
}

void DisplayUI::createMenu(Menu* menu, Menu* parent, std::function<void()>build) {
  menu->list       = new SimpleList<MenuNode>;
  menu->parentMenu = parent;
  menu->selected   = 0;
  menu->build      = build;
}

void DisplayUI::addMenuNode(Menu* menu, std::function<String()>getStr, std::function<void()>click, std::function<void()>hold) {
  menu->list->add(MenuNode{ getStr, click, hold });
}

void DisplayUI::addMenuNode(Menu* menu, std::function<String()>getStr, std::function<void()>click) {
  addMenuNode(menu, getStr, click, NULL);
}

void DisplayUI::addMenuNode(Menu* menu, std::function<String()>getStr, Menu* next) {
  addMenuNode(menu, getStr, [this, next]() {
    changeMenu(next);
  });
}

void DisplayUI::addMenuNode(Menu* menu, const char* ptr, std::function<void()>click) {
  addMenuNode(menu, [ptr]() {
    return str(ptr);
  }, click);
}

void DisplayUI::addMenuNode(Menu* menu, const char* ptr, Menu* next) {
  addMenuNode(menu, [ptr]() {
    return str(ptr);
  }, next);
}

void DisplayUI::setTime(int h, int m, int s) {
  if (s >= 60) {
    s = 0;
    m++;
  }

  if (m >= 60) {
    m = 0;
    h++;
  }

  if (h >= 24) {
    h = 0;
  }

  if (s < 0) {
    s = 59;
    m--;
  }

  if (m < 0) {
    m = 59;
    h--;
  }

  if (h < 0) {
    h = 23;
  }

  clockHour   = h;
  clockMinute = m;
  clockSecond = s;

  #ifdef RTC_DS3231
    clock.setHour(clockHour);
    clock.setMinute(clockMinute);
    clock.setSecond(clockSecond);
  #endif // ifdef RTC_DS3231
}