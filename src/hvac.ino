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

const int kBridgeSenseNPin = PA3;
const int kBridgeSensePPin = PA2;

// Tuning constants
constexpr uint16_t kUpdateTempMs = 1000;
constexpr uint32_t kIdleBacklightOffMs = 5000;

// State
uint32_t update_display_at = 0;
uint32_t backlight_off_at = 0;

const constexpr int kMaxDepth = 4;

// Menu settings
int8_t set_temp = 60;
uint8_t fan_speed = 180;
uint8_t pump_speed = 255;
int8_t swamp_threshold = 70;

// Config options
bool sound_on = true;
bool mode_auto = true;
bool use_water_switch = true;

uint16_t fan_sense = 0;
uint16_t pump_sense = 0;
uint16_t bridge_sense_n = 0;
uint16_t bridge_sense_p = 0;

LiquidCrystal_I2C lcd(/*address=*/0x27, /*columns=*/16, /*rows=*/2);
// TODO: update button to use the encoder button
ClickEncoder clickEncoder(kEncAPin, kEncBPin, kEncButtonPin,
                          /*stepsPerNotch=*/4);
ClickEncoderStream encStream(clickEncoder, 1);

result FanChanged();

// clang-format off
TOGGLE(sound_on, sound_on_menu, "Sound:", doNothing, noEvent, wrapStyle
  ,VALUE("On", true, doNothing, noEvent)
  ,VALUE("Off", false, doNothing, noEvent)
);

TOGGLE(use_water_switch, water_switch_menu, "WaterSwitch:", doNothing, noEvent, wrapStyle
  ,VALUE("Yes", true, doNothing, noEvent)
  ,VALUE(" No", false, doNothing, noEvent)
);

MENU(config_menu,"Config",doNothing,noEvent,wrapStyle
  ,SUBMENU(water_switch_menu)
  ,SUBMENU(sound_on_menu)
  ,EXIT("...")
);

MENU(sense_menu,"Sense",doNothing,noEvent,wrapStyle
  ,FIELD(fan_sense, "Fan sense", "", 0, 4096, 0, 0, doNothing, noEvent, wrapStyle)
  ,FIELD(pump_sense, "Pump sense", "", 0, 4096, 0, 0, doNothing, noEvent, wrapStyle)
  ,FIELD(bridge_sense_n, "Bridge P", "", 0, 4096, 0, 0, doNothing, noEvent, wrapStyle)
  ,FIELD(bridge_sense_p, "Bridge N", "", 0, 4096, 0, 0, doNothing, noEvent, wrapStyle)
  ,EXIT("...")
);

TOGGLE(mode_auto, mode_menu, "Mode:", doNothing, noEvent, wrapStyle
  ,VALUE("Auto", true, doNothing, noEvent)
  ,VALUE("Manual", false, doNothing, noEvent)
);

MENU(main_menu,"Main menu",doNothing,noEvent,wrapStyle
  ,FIELD(set_temp, "Temp", "F", 30, 100, 10, 1, doNothing, noEvent, wrapStyle)
  ,FIELD(fan_speed, "Fan", "", 0, 255, 25, 1, FanChanged, updateEvent, wrapStyle)
  ,FIELD(pump_speed, "Pump", "", 0, 255, 25, 1, doNothing, updateEvent, wrapStyle)
  ,FIELD(swamp_threshold, "Swamp Thresh", "F", 30, 100, 10, 1, doNothing, noEvent, wrapStyle)
  ,SUBMENU(mode_menu)
  ,SUBMENU(config_menu)
  ,SUBMENU(sense_menu)
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
Menu::navRoot nav(main_menu, nav_cursors, kMaxDepth - 1, in, out);

ArduinoInputs inputs;
ArduinoOutputs outputs;

bool in_idle = false;
bool invalid = false;

STM32Timer ITimer(TIM3);
STM32_ISR_Timer ISR_Timer;

void TimerHandler() {
  ISR_Timer.run();
  outputs.Tick();
}

void ServiceEncoder() {
  // digitalWrite(kLedPin, HIGH);
  clickEncoder.service();
  // digitalWrite(kLedPin, LOW);
}

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
  if (sound_on) {
    tone(kBuzzerPin, 4000, 100);
  }
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
    if (on && sound_on) {
      tone(kBuzzerPin, 4000, 500);
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

  pinMode(kBridgeSenseNPin, INPUT);
  pinMode(kBridgeSensePPin, INPUT);

  if (!inputs.Init()) {
    Serial.println("inputs.Init() failed");
    Warning();
    invalid = true;
  }
  if (!outputs.Init()) {
    Serial.println("outputs.Init() failed");
    Warning();
  }
  outputs.SetFanDirection(true);

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

  // 75uS * 256 ~= 50Hz
  if (!ITimer.attachInterruptInterval(75, TimerHandler)) {
    FatalError();
  }
  // Poll the encoder every 1ms
  ISR_Timer.setInterval(1, ServiceEncoder);

  // Make sense display read-only
  sense_menu[0].disable();
  sense_menu[1].disable();
  sense_menu[2].disable();
  sense_menu[3].disable();
}

void loop() {
  if (in_idle && millis() > update_display_at) {
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

    lcd.setCursor(/*col=*/0, /*row=*/1);
    lcd.print("Fan ");
    uint8_t fan = outputs.GetFan();
    if (fan == 255) {
      lcd.print(" ON");
    } else if (fan == 0) {
      lcd.print("OFF");
    } else {
      lcd.print(fan);
    }

    lcd.print(" Pump ");
    uint8_t pump = outputs.GetPump();
    if (pump == 255) {
      lcd.print(" ON");
    } else if (pump == 0) {
      lcd.print("OFF");
    } else {
      lcd.print(pump);
    }

    update_display_at = millis() + kUpdateTempMs;
  }

  if (mode_auto) {
    bool missing_water = false;
    // TODO: how should temp control behave when a temp is invalid?
    bool new_invalid = false;
    float inside = inputs.GetInside();
    if (inside <= ArduinoInputs::kNoTemp) {
      if (!invalid) {
        Warning();
      }
      invalid = true;
      new_invalid = true;
    }
    float outside = inputs.GetOutside();
    if (outside <= ArduinoInputs::kNoTemp) {
      if (!invalid) {
        Warning();
      }
      invalid = true;
      new_invalid = true;
    }

    if (inside > set_temp) {
      if (outside > inside) {
        if (outside > swamp_threshold) {
          if (use_water_switch) {
            if (inputs.GetWaterSwitch()) {
              outputs.SetPump(pump_speed);
              outputs.SetFan(fan_speed);
            } else {
              // Swamp is requested, but not available.
              outputs.SetFan(0);
              outputs.SetPump(0);
              missing_water = true;
            }
          } else {
            // No water switch
            outputs.SetPump(pump_speed);
            outputs.SetFan(fan_speed);
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
          if (use_water_switch) {
            if (inputs.GetWaterSwitch()) {
              outputs.SetPump(pump_speed);
            } else {
              // Swamp is requested, but not available.
              missing_water = true;
              outputs.SetPump(0);
            }
          } else {
            // No water switch
            outputs.SetPump(pump_speed);
          }
        }
      }
    } else {
      // inside < set_temp
      outputs.SetFan(0);
      outputs.SetPump(0);
    }

    invalid = new_invalid;

    if (use_water_switch) {
      digitalWrite(kLedPin, missing_water);
    }
  } else {
    // Manual mode
    if (use_water_switch && !inputs.GetWaterSwitch()) {
    } else {
      outputs.SetFan(fan_speed);
      outputs.SetPump(pump_speed);
    }
  }

  if (backlight_off_at && millis() > backlight_off_at) {
    // lcd.noBacklight();
    backlight_off_at = 0;
  }

  fan_sense = analogRead(kFanSensePin);
  pump_sense = analogRead(kPumpSensePin);
  bridge_sense_n = analogRead(kBridgeSenseNPin);
  bridge_sense_p = analogRead(kBridgeSensePPin);

  nav.poll();
  delay(1);
}