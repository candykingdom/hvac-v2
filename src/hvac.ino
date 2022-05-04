#include <menu.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

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
  switch (e) {
    case idleStart:
      break;
    case idling:
      o.println("idle");
      break;
    case idleEnd:
      nav.reset();
      break;
  }
  return proceed;
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  nav.timeOut=10;
  nav.idleTask=idle;
}

void loop() {
  nav.poll();
  delay(10);
}