#include <menu.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

#include "temps.h"

// Pin definitions
const int kLedPin = 13;

// Tuning constants
const uint16_t kUpdateTempMs = 1000;

// State
uint32_t update_temp_at = 0;

const constexpr int kMaxDepth = 3;

int test = 0;

// clang-format off
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,EXIT("...")
);
// clang-format on

serialIn in(Serial);

const Menu::panel panels[] MEMMODE = {{0, 0, 16, 2}};
Menu::navNode *nodes[sizeof(panels) / sizeof(Menu::panel)];
Menu::panelsList pList(panels, nodes, 1);

Menu::idx_t serialTops[kMaxDepth];
Menu::serialOut outSerial(Serial, serialTops);

Menu::menuOut *outputs[] MEMMODE = {&outSerial};
Menu::outputsList out(outputs, 1);

// aux objects to control each level of navigation
Menu::navNode nav_cursors[kMaxDepth];
Menu::navRoot nav(mainMenu, nav_cursors, kMaxDepth - 1, in, out);

result idle(menuOut &o, idleEvent e) {
  Serial.println(e);
  switch (e) {
    case idleStart:
    o.println("idleStart");
      break;
    case idling:
      o.print("Out: ");
      o.println(temps::GetOutside(), 3);
      nav.idleChanged = true;
      break;
    case idleEnd:
      nav.reset();
      break;
  }
  return proceed;
}

void setup() {
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, HIGH);

  temps::Init();

  Serial.begin(9600);
  while (!Serial)
    ;
  nav.timeOut = 10;
  nav.idleTask = idle;

  digitalWrite(kLedPin, LOW);
}

void loop() {
  if (millis() > update_temp_at) {
    Serial.println("run");
    out.idle()
    nav.idleChanged = !nav.idleChanged;
    update_temp_at = millis() + kUpdateTempMs;
  }

  nav.poll();
  delay(100);
}