#include <menu.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

#include "inputs.h"
#include "fake-inputs.h"
#include "arduino-inputs.h"
#include "outputs.h"
#include "arduino-outputs.h"
#include "fake-outputs.h"

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

Menu::menuOut *menu_outputs[] MEMMODE = {&outSerial};
Menu::outputsList out(menu_outputs, 1);

// aux objects to control each level of navigation
Menu::navNode nav_cursors[kMaxDepth];
Menu::navRoot nav(mainMenu, nav_cursors, kMaxDepth - 1, in, out);

FakeInputs inputs;
FakeOutputs outputs;

bool in_idle = false;

result idle(menuOut &o, idleEvent e) {
  switch (e) {
    case idleStart:
      o.clear();
      in_idle = true;
      // TODO: remove?
      Serial.println();
      break;
    case idling:
      break;
    case idleEnd:
      nav.reset();
      in_idle = false;
      break;
  }
  return proceed;
}

void setup() {
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, HIGH);

  if (!inputs.Init()) {
    Serial.println("inputs.Init() failed");
  }
  if (!outputs.Init()) {
    Serial.println("outputs.Init() failed");
  }

  Serial.begin(9600);
  while (!Serial)
    ;
  nav.timeOut = 10;
  nav.idleTask = idle;

  digitalWrite(kLedPin, LOW);
}

void loop() {
  if (in_idle && millis() > update_temp_at) {
    // TODO: output to screen
    Serial.print("Out ");
    Serial.print(inputs.GetOutside(), /* digits= */ 0);
    Serial.print("   In ");
    Serial.println(inputs.GetInside(), /* digits= */ 0);
    update_temp_at = millis() + kUpdateTempMs;
  }

  nav.poll();
  delay(10);
}