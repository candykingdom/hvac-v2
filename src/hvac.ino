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
#include <FlashStorage_STM32.h>

#include "arduino-inputs.h"
#include "arduino-outputs.h"
#include "fake-outputs.h"
#include "inputs.h"
#include "outputs.h"
#include "runner.h"

// Pin definitions
constexpr int kBuzzerPin = PB1;
constexpr int kEncAPin = PB14;
constexpr int kEncBPin = PB15;
// TODO: switch to actual encoder button once its connected to the micro
// constexpr int kEncButtonPin = PB11; // v2.3 with hotfix
constexpr int kEncButtonPin = PA4;  // v2.4

constexpr int kFanSensePin = PA6;
constexpr int kPumpSensePin = PA1;
constexpr int kBridgeSenseNPin = PA3;
constexpr int kBridgeSensePPin = PA2;

// Tuning constants
constexpr uint16_t kUpdateTempMs = 1000;
constexpr uint32_t kIdleBacklightOffMs = 30 * 1000;

// State
uint32_t update_display_at = 0;
uint32_t backlight_off_at = 0;

const constexpr int kMaxDepth = 4;

constexpr FanType kFanType = FanType::MOSFET_BANGBANG;
ArduinoInputs inputs;
ArduinoOutputs outputs(kFanType);

RunnerParams runner_params;
Runner runner(runner_params, inputs, outputs);

uint16_t fan_sense = 0;
uint16_t pump_sense = 0;
uint16_t bridge_sense_n = 0;
uint16_t bridge_sense_p = 0;

LiquidCrystal_I2C lcd(/*address=*/0x27, /*columns=*/16, /*rows=*/2);
ClickEncoder clickEncoder(kEncAPin, kEncBPin, kEncButtonPin,
                          /*stepsPerNotch=*/4);
ClickEncoderStream encStream(clickEncoder, 1);

void SaveConfig() {
  EEPROM.put(0, runner_params);
}

// clang-format off
TOGGLE(runner_params.sound_on, sound_on_menu, "Sound:", doNothing, noEvent, wrapStyle
  ,VALUE("On", true, doNothing, noEvent)
  ,VALUE("Off", false, doNothing, noEvent)
);

TOGGLE(runner_params.use_water_switch, water_switch_menu, "WaterSwitch:", doNothing, noEvent, wrapStyle
  ,VALUE("Yes", true, doNothing, noEvent)
  ,VALUE(" No", false, doNothing, noEvent)
);

TOGGLE(runner_params.swamp_direction, swamp_direction_menu, "SwampDir:", doNothing, noEvent, wrapStyle
  ,VALUE("Forw", true, doNothing, noEvent)
  ,VALUE("Back", false, doNothing, noEvent)
);

TOGGLE(runner_params.vent_direction, vent_direction_menu, "VentDir:", doNothing, noEvent, wrapStyle
  ,VALUE("Forw", true, doNothing, noEvent)
  ,VALUE("Back", false, doNothing, noEvent)
);

MENU(config_menu,"Config",doNothing,noEvent,wrapStyle
  ,FIELD(runner_params.temp_deadband, "Temp Band", "F", 0, 20, 1, 0.1, doNothing, noEvent, wrapStyle)
  ,SUBMENU(water_switch_menu)
  ,FIELD(runner_params.pump_period, "Pump Cycle", "s", 0, 999, 60, 5, doNothing, noEvent, wrapStyle)
  ,FIELD(runner_params.pump_duty, "Pump Duty", "", 0, 255, 25, 1, doNothing, noEvent, wrapStyle)
  ,SUBMENU(swamp_direction_menu)
  ,SUBMENU(vent_direction_menu)
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

TOGGLE(runner_params.run_mode, mode_menu, "Mode:", doNothing, noEvent, wrapStyle
  ,VALUE("Auto", RunMode::AUTO, doNothing, noEvent)
  ,VALUE("Swamp", RunMode::SWAMP, doNothing, noEvent)
  ,VALUE("Vent", RunMode::VENT, doNothing, noEvent)
  ,VALUE("Off", RunMode::OFF, doNothing, noEvent)
);

MENU(main_menu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(mode_menu)
  ,FIELD(runner_params.set_temp, "Temp", "F", 30, 100, 10, 1, doNothing, noEvent, wrapStyle)
  ,FIELD(runner_params.swamp_fan_speed, "SwampFan", "", 0, 255, 25, 1, doNothing, updateEvent, wrapStyle)
  ,FIELD(runner_params.vent_fan_speed, "VentFan ", "", 0, 255, 25, 1, doNothing, updateEvent, wrapStyle)
  ,FIELD(runner_params.pump_speed, "Pump", "", 0, 255, 25, 1, doNothing, updateEvent, wrapStyle)
  ,FIELD(runner_params.swamp_threshold, "Swamp Thresh", "F", 30, 100, 10, 1, doNothing, noEvent, wrapStyle)
  ,SUBMENU(config_menu)
  ,SUBMENU(sense_menu)
  ,OP("Save", SaveConfig, enterEvent)
  ,EXIT("...")
);

// clang-format on

MENU_INPUTS(in, &encStream);
const Menu::panel panels[] MEMMODE = {{0, 0, 16, 2}};
Menu::navNode *nodes[sizeof(panels) / sizeof(Menu::panel)];
Menu::panelsList pList(panels, nodes, 1);

Menu::idx_t serialTops[kMaxDepth];
Menu::liquidCrystalOut outLCD(lcd, serialTops, pList);

Menu::menuOut *menu_outputs[] MEMMODE = {&outLCD};
Menu::outputsList out(menu_outputs, 1);

// aux objects to control each level of navigation
Menu::navNode nav_cursors[kMaxDepth];
Menu::navRoot nav(main_menu, nav_cursors, kMaxDepth - 1, in, out);

bool in_idle = false;

STM32Timer ITimer(TIM3);
STM32_ISR_Timer ISR_Timer;

void TimerHandler() {
  ISR_Timer.run();
  outputs.Tick();
}

void ServiceEncoder() {
  // outputs.SetLed(true);
  clickEncoder.service();
  // outputs.SetLed(false);
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

// In case of non-recoverable error, fast-blink the LED
void FatalError() {
  bool on = true;
  if (runner_params.sound_on) {
    tone(kBuzzerPin, 4000, 100);
  }
  while (true) {
    outputs.SetLed(on);
    on = !on;
    delay(200);
  }
}

void Warning() {
  bool on = true;
  for (int i = 0; i < 4; i++) {
    outputs.SetLed(on);
    if (on && runner_params.sound_on) {
      tone(kBuzzerPin, 4000, 500);
    }
    on = !on;
    delay(500);
  }
}

void setup() {
  // If the flash storage has been written before, load config from it.
  // Note: this seems to return true always, so we double-check using `runner_params.valid`.
  if (EEPROM.isValid()) {
    EEPROM.get(0, runner_params);
  }
  if (runner_params.valid != RunnerParams::kValid) {
    runner_params.SetDefaults();
  }
  
  if (!outputs.Init()) {
    Serial.println("outputs.Init() failed");
    Warning();
  }

  pinMode(kBuzzerPin, OUTPUT);
  pinMode(kEncAPin, INPUT);
  pinMode(kEncBPin, INPUT);
  pinMode(kEncButtonPin, INPUT_PULLUP);

  pinMode(kFanSensePin, INPUT);
  pinMode(kPumpSensePin, INPUT);

  pinMode(kBridgeSenseNPin, INPUT);
  pinMode(kBridgeSensePPin, INPUT);

  if (!inputs.Init()) {
    Serial.println("inputs.Init() failed");
    Warning();
  }
  nav.timeOut = 10;
  nav.idleTask = idle;
  nav.useUpdateEvent = true;

  Wire.setSCL(PF6);
  Wire.setSDA(PF7);
  lcd.init();
  lcd.backlight();

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
      lcd.print("NA   ");
    } else {
      lcd.print(inputs.GetInside(), /* digits= */ 0);
      lcd.print("   ");
    }

    lcd.setCursor(/*col=*/0, /*row=*/1);
    if (runner_params.run_mode == RunMode::OFF) {
      lcd.print("Off");
    } else if ((runner_params.run_mode == RunMode::AUTO ||
                runner_params.run_mode == RunMode::SWAMP) &&
               runner_params.use_water_switch && !inputs.GetWaterSwitch()) {
      lcd.print("No Water!        ");
    } else {
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
    }

    update_display_at = millis() + kUpdateTempMs;
  }

  if (backlight_off_at && millis() > backlight_off_at) {
    lcd.noBacklight();
    backlight_off_at = 0;
  }

  runner.Tick();

  fan_sense = analogRead(kFanSensePin);
  pump_sense = analogRead(kPumpSensePin);
  bridge_sense_n = analogRead(kBridgeSenseNPin);
  bridge_sense_p = analogRead(kBridgeSensePPin);

  nav.poll();
}