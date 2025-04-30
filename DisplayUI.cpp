#include <Arduino.h>

#include "Config.hpp"
#include "DisplayUI.hpp"
#include "StartupLogo.hpp"

void strToColor(String str, uint8_t* buf) {
    str.replace(":", "");
    str.replace("0x", "");
    str.replace(",", "");
    str.replace("#", "");
    str.toUpperCase();

    if (str.length() != 6) {
        prntln(F_COLOR_INVALID);
        return;
    }

    for (uint8_t i = 0; i < 3; i++) buf[i] = strtoul((str.substring(i * 2, i * 2 + 2)).c_str(), NULL, 16);
}

String center(String a, int len) {
    int spaces = len - a.length();

    for (int i = 0; i < spaces; i += 2) {
        a = ' ' + a + ' ';
    }

    a = a.substring(0, len);

    return a;
}

String left(String a, int len) {
    int spaces = len - a.length();

    while (spaces > 0) {
        a = a + ' ';
        spaces--;
    }

    a = a.substring(0, len);

    return a;
}

String right(String a, int len) {
    int spaces = len - a.length();

    while (spaces > 0) {
        a = ' ' + a;
        spaces--;
    }

    a = a.substring(0, len);

    return a;
}

String leftRight(String a, String b, int len) {
    int spaces = len - a.length() - b.length();

    while (spaces > 0) {
        a = a + ' ';
        spaces--;
    }

    a = a + b;

    a = a.substring(0, len);

    return a;
}

String escape(String str) {
    str.replace(String(BACKSLASH), String(BACKSLASH) + String(BACKSLASH));
    str.replace(String(DOUBLEQUOTES), String(BACKSLASH) + String(DOUBLEQUOTES));
    return str;
}

bool ascii(char c) {
    return c >= 0 && c <= 127;
}

bool printableAscii(char c) {
    return c >= 32 && c <= 126;
}

bool getBit(uint8_t b, uint8_t n) {
    return (b >> n) % 2 != 0;
}

uint8_t utf8(uint8_t c) {
    if (!getBit(c, 7)) return 1;

    if (getBit(c, 7) && getBit(c, 6) && !getBit(c, 5)) return 2;

    if (getBit(c, 7) && getBit(c, 6) && getBit(c, 5) && !getBit(c, 4)) return 3;

    if (getBit(c, 7) && getBit(c, 6) && getBit(c, 5) && getBit(c, 4) && !getBit(c, 3)) return 4;

    return 0;
}

bool utf8Part(uint8_t c) {
    return getBit(c, 7) && !getBit(c, 6);
}

String fixUtf8(String str) {
    int size = str.length();

    String  result = String();
    char    c;
    uint8_t len;
    bool    ok;

    for (int i = 0; i < size; i++) {
        c   = str.charAt(i);       // get character
        len = utf8(c);             // get utf8 char len

        if (len <= 1) {
            result += c;           // when 1 byte char, add it :)
        }
        else if (i + len > size) { // when char bigger than remaining string, end loop
            i = size + 1;
        }
        else {
            ok = true;

            for (int j = 1; j < len && ok; j++) {
                ok = utf8Part(str.charAt(i + j));   // if following char is compliant or not
            }

            if (ok) result += c;                    // everything is ok, add char and continue
            else {                                  // utf8 char is broken
                for (int j = 1; j < len; j++) {     // go through the next bytes
                    c = str.charAt(i + j);

                    if (utf8(c) == 1) result += c;  // when byte is ascii, add it :)
                }
                i += len - 1;                       // skip utf8 char because we already managed it
            }
        }
    }
    return result;
}

String removeUtf8(String str) {
    str = fixUtf8(str); // fix it in case a utf char is broken
    int size = str.length();

    String  result = String();
    char    c;
    uint8_t len;

    for (int i = 0; i < size; i++) {
        c   = str.charAt(i);        // get character
        len = utf8(c);              // get utf8 char len

        if (len <= 1) result += c;  // when 1 byte char, add it :)
        else i += len - 1;          // skip other chars
    }

    return result;
}

int utf8Len(String str) {
    int size = str.length();

    int  result = 0;
    char c;
    uint8_t len;

    for (int i = 0; i < size; i++) {
        c   = str.charAt(i);     // get character
        len = utf8(c);           // get utf8 char len

        if (len <= 1) result++;  // when 1 byte char, add 1 :)
        else {
            result++;

            for (int j = 1; j < len; j++) {
                c = str.charAt(i + j);

                if (!utf8Part(c) && (utf8(c) == 1)) {
                    Serial.println(c, HEX);
                    result++; // if following char is compliant or not
                }
            }
            i += len - 1;
        }
    }

    return result;
}

String replaceUtf8(String str, String r) {
    str = fixUtf8(str); // fix it in case a utf char is broken
    int size = str.length();

    String  result = String();
    char    c;
    uint8_t len;

    for (int i = 0; i < size; i++) {
        c   = str.charAt(i);        // get character
        len = utf8(c);              // get utf8 char len

        if (len <= 1) result += c;  // when 1 byte char, add it :)
        else {
            result += r;
            i      += len - 1;      // skip other chars
        }
    }

    return result;
}

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

/*
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
*/

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

DisplayUI::DisplayUI() {}

DisplayUI::~DisplayUI() {}

void DisplayUI::setup() {
  configInit();
  //setupButtons();
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