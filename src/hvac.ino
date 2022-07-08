#include <LiquidCrystal_I2C.h>
#include <STM32TimerInterrupt.h>
#include <STM32_ISR_Timer.h>
#include <Tone.h>
#include <Wire.h>
#include <menu.h>
#include <menuIO/chainStream.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/liquidCrystalOutI2C.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

#include "arduino-inputs.h"
#include "arduino-outputs.h"
#include "fake-outputs.h"
#include "inputs.h"
#include "outputs.h"

// Pin definitions
const int kLedPin = PA11;
const int kBuzzerPin = PB1;
const int kEncAPin = PB14;
const int kEncBPin = PB15;
// TODO: switch to actual encoder button once its connected to the micro
const int kEncButtonPin = PB11;

const int kFanOutputPin = PA9;
const int kFanSensePin = PA6;
const int kPumpOutputPin = PA10;
const int kPumpSensePin = PA1;

// Tuning constants
constexpr uint16_t kUpdateTempMs = 1000;
constexpr uint32_t kIdleBacklightOffMs = 5000;

// State
uint32_t update_temp_at = 0;
uint32_t backlight_off_at = 0;

const constexpr int kMaxDepth = 3;

int8_t set_temp = 60;
uint8_t fan_speed = 255;
int8_t swamp_threshold = 70;

LiquidCrystal_I2C lcd(/*address=*/0x27, /*columns=*/16, /*rows=*/2);
// TODO: update button to use the encoder button
ClickEncoder clickEncoder(kEncAPin, kEncBPin, kEncButtonPin,
                          /*stepsPerNotch=*/4);
ClickEncoderStream encStream(clickEncoder, 1);

result FanChanged();

// clang-format off
MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(set_temp, "Temp", "F", 30, 100, 10, 1, doNothing, noEvent, wrapStyle)
  ,FIELD(fan_speed, "Fan", "", 0, 255, 25, 1, FanChanged, updateEvent, wrapStyle)
  ,FIELD(swamp_threshold, "Swamp Thresh", "F", 30, 100, 10, 1, doNothing, noEvent, wrapStyle)
  ,EXIT("...")
);
// clang-format on

MENU_INPUTS(in, &encStream);
const Menu::panel panels[] MEMMODE = {{0, 0, 16, 2}};
Menu::navNode *nodes[sizeof(panels) / sizeof(Menu::panel)];
Menu::panelsList pList(panels, nodes, 1);

Menu::idx_t serialTops[kMaxDepth];
// Menu::serialOut outSerial(Serial, serialTops);
Menu::liquidCrystalOut outLCD(lcd, serialTops, pList);

Menu::menuOut *menu_outputs[] MEMMODE = {&outLCD};
Menu::outputsList out(menu_outputs, 1);

// aux objects to control each level of navigation
Menu::navNode nav_cursors[kMaxDepth];
Menu::navRoot nav(mainMenu, nav_cursors, kMaxDepth - 1, in, out);

ArduinoInputs inputs;
FakeOutputs outputs;

bool in_idle = false;
bool invalid = false;

STM32Timer ITimer0(TIM1);
void ServiceEncoder() { clickEncoder.service(); }

result idle(menuOut &o, idleEvent e) {
  switch (e) {
    case idleStart:
      o.clear();
      in_idle = true;
      backlight_off_at = millis() + kIdleBacklightOffMs;
      break;
    case idling:
      break;
    case idleEnd:
      nav.reset();
      in_idle = false;
      backlight_off_at = 0;
      lcd.backlight();
      break;
  }
  return proceed;
}

result FanChanged() {
  Serial.println("Fan changed");
  if (outputs.GetFan() > 0) {
    outputs.SetFan(fan_speed);
  }
  return proceed;
}

// In case of non-recoverable error, fast-blink the LED
void FatalError() {
  bool on = true;
  while (true) {
    digitalWrite(kLedPin, on);
    on = !on;
    delay(200);
  }
}

void Warning() {
  bool on = true;
  for (int i = 0; i < 4; i++) {
    digitalWrite(kLedPin, on);
    if (on) {
      tone(kBuzzerPin, 2000, 500);
    }
    on = !on;
    delay(500);
  }
}

void setup() {
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kLedPin, HIGH);

  pinMode(kBuzzerPin, OUTPUT);
  pinMode(kEncAPin, INPUT);
  pinMode(kEncBPin, INPUT);
  pinMode(kEncButtonPin, INPUT_PULLUP);

  pinMode(kFanOutputPin, OUTPUT);
  pinMode(kFanSensePin, INPUT);
  pinMode(kPumpOutputPin, OUTPUT);
  pinMode(kPumpSensePin, INPUT);

  if (!inputs.Init()) {
    Serial.println("inputs.Init() failed");
    Warning();
    invalid = true;
  }
  if (!outputs.Init()) {
    Serial.println("outputs.Init() failed");
    Warning();
  }

  Serial.begin(9600);
  while (!Serial)
    ;
  nav.timeOut = 10;
  nav.idleTask = idle;
  nav.useUpdateEvent = true;

  Wire.setSCL(PF6);
  Wire.setSDA(PF7);
  lcd.init();
  lcd.backlight();

  digitalWrite(kLedPin, LOW);

  analogWriteResolution(8);
  // Note: don't make this too high, or it might fry the board!
  analogWriteFrequency(50);

  if (!ITimer0.attachInterruptInterval(1 * 1000, ServiceEncoder)) {
    FatalError();
  }
}

void loop() {
  if (in_idle && millis() > update_temp_at) {
    lcd.home();
    lcd.print("Out ");
    float outside = inputs.GetOutside();
    if (outside <= ArduinoInputs::kNoTemp) {
      lcd.print("NA");
    } else {
      lcd.print(inputs.GetOutside(), /* digits= */ 0);
    }

    lcd.print("   In ");
    float inside = inputs.GetInside();
    if (inside <= ArduinoInputs::kNoTemp) {
      lcd.print("NA");
    } else {
      lcd.print(inputs.GetInside(), /* digits= */ 0);
    }
    update_temp_at = millis() + kUpdateTempMs;
  }

  bool missing_water = false;
  bool new_invalid = false;
  float inside = inputs.GetInside();
  if (inside <= ArduinoInputs::kNoTemp) {
    if (!invalid) {
      Warning();
    }
    invalid = true;
    new_invalid = true;
  }
  if (inside > set_temp) {
    float outside = inputs.GetOutside();
    if (outside <= ArduinoInputs::kNoTemp) {
      if (!invalid) {
        Warning();
      }
      invalid = true;
      new_invalid = true;
    }
    if (outside > inside) {
      if (outside > swamp_threshold) {
        if (inputs.GetWaterSwitch()) {
          outputs.SetPump(255);
          outputs.SetFan(fan_speed);
        } else {
          // Swamp is requested, but not available.
          outputs.SetFan(0);
          outputs.SetPump(0);
          missing_water = true;
        }
      } else {
        // outside < swamp_threshold
        outputs.SetFan(0);
        outputs.SetPump(0);
      }
    } else {
      // outside < inside
      outputs.SetFan(fan_speed);
      if (outside > swamp_threshold) {
        if (inputs.GetWaterSwitch()) {
          outputs.SetPump(255);
        } else {
          // Swamp is requested, but not available.
          missing_water = true;
          outputs.SetPump(0);
        }
      }
    }
  } else {
    // inside < set_temp
    outputs.SetFan(0);
    outputs.SetPump(0);
  }

  invalid = new_invalid;

  digitalWrite(kLedPin, missing_water);

  if (backlight_off_at && millis() > backlight_off_at) {
    // lcd.noBacklight();
    backlight_off_at = 0;
  }

  nav.poll();
  delay(10);
}