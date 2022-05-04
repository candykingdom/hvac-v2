#include <FastLED.h>

// https://github.com/PaulStoffregen/OneWire
#include <OneWire.h>
// https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DallasTemperature.h>
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>

const int kNumLeds = 3;

const int kNapButtonPin = 9;
const int kAuxButtonPin = 10;
const int kSpeedPin = A2;
const int kIntTempPin = 11;
const int kExtTempPin = 12;
const int kWaterPin = 13;
const int kFanPin = 6;
const int kPumpPin = 7;
const int kFanPwmPin = 8;

CRGB leds[kNumLeds];

// Temp sensors
OneWire intOneWire(kIntTempPin);
DallasTemperature intTemp(&intOneWire);
OneWire extOneWire(kExtTempPin);
DallasTemperature extTemp(&extOneWire);

// Buttons
Bounce napButton;
Bounce auxButton;

// Fan speeds (in percent)
const int speeds[] = {100, 80, 60};
const int kNumSpeeds = 3;
int currentSpeedIndex = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  FastLED.addLeds<SK6812, 2, GRB>(leds, kNumLeds);
  FastLED.setBrightness(16);
  FastLED.showColor(CRGB(0, 0, 0));

  Serial.begin(112500);
  pinMode(kSpeedPin, INPUT);
  pinMode(kIntTempPin, INPUT_PULLUP);
  pinMode(kExtTempPin, INPUT_PULLUP);
  pinMode(kWaterPin, INPUT_PULLUP);
  pinMode(kFanPin, OUTPUT);
  pinMode(kPumpPin, OUTPUT);
  pinMode(kFanPwmPin, OUTPUT);

  intTemp.begin();
  extTemp.begin();
  napButton.attach(kNapButtonPin, INPUT_PULLUP);
  auxButton.attach(kAuxButtonPin, INPUT_PULLUP);

  // Set initial fan speed to full
  analogWrite(kFanPwmPin, 255);
}


// millis to run the temp read/swamp control loop at
unsigned long tempLoopRunAt = 0;
// TODO
//const unsigned long kRunTempLoopEvery = 30 * 1000;
const unsigned long kRunTempLoopEvery = 5 * 1000;
// When it's hotter than this outside, always run the swamp cooler (even if it's cooler outside than inside).
const float kSwampCoolerTempThresh = 70;

void loop() {
  // Make sure that we can talk to the temp sensors
  int intDeviceCount = intTemp.getDeviceCount();
  int extDeviceCount = extTemp.getDeviceCount();
  if (!intDeviceCount || !extDeviceCount) {
    Serial.println("Temp sensor missing");
    // Flash purple
    uint8_t brightness = (millis() / 500) % 2 ? 255 : 0;
    leds[0] = CRGB(brightness, 0, brightness);
    FastLED.show();

    // This is important for logging
    delay(100);

    return;
  }

  // Fan speed control
  if (auxButton.update() && auxButton.fell()) {
    leds[0] = CRGB(0, 0, 255);
    FastLED.show();
    currentSpeedIndex = (currentSpeedIndex + 1) % kNumSpeeds;
    int speed = speeds[currentSpeedIndex] * 255 / 100;
    analogWrite(kFanPwmPin, speed);
    Serial.print("Fan speed: ");
    Serial.println(speed);
    delay(5);
  }

  // Don't do this check inside the temp loop, because we want to kill the pump as soon as there's no water.
  const bool waterPresent = !digitalRead(kWaterPin);
  if (!waterPresent) {
    // Red warning light
    leds[0] = CRGB(255, 0, 0);
    FastLED.show();

    digitalWrite(kPumpPin, LOW);
  } else {
    leds[0] = CRGB(0, 0, 16);
    FastLED.show();
  }
  
  if (tempLoopRunAt < millis()) {
    // Temp loop
    intTemp.requestTemperatures();
    extTemp.requestTemperatures();
    const float intF = intTemp.getTempFByIndex(0);
    const float extF = extTemp.getTempFByIndex(0);
    
    Serial.print(intF);
    Serial.print("F, ext: ");
    Serial.print(extF);
    Serial.println("F");

    if (intF > extF && extF < kSwampCoolerTempThresh) {
      // Inside is hotter and it's cold outside, so just run the fan. We can still do this even if there's no water.
      Serial.println("Running fan only");
      digitalWrite(kFanPin, HIGH);
      digitalWrite(kPumpPin, LOW);
    } else if (!waterPresent) {
      // No water and it's hot, so just turn off.
      Serial.println("Off");
      digitalWrite(kFanPin, LOW);
      digitalWrite(kPumpPin, LOW);
    } else {
      // It's hot, so run the swamp cooler
      Serial.println("Running swamp");
      digitalWrite(kFanPin, HIGH);
      digitalWrite(kPumpPin, HIGH);
    }

    tempLoopRunAt = millis() + kRunTempLoopEvery;
  }

  delay(1);


  /*intTemp.requestTemperatures();
  Serial.println(intTemp.getTempFByIndex(0));*/

  /*analogWrite(kFanPwmPin, 10);*/

  /*if (napButton.update() && napButton.read()) {
    on = !on;
    if (on) {
      analogWrite(kFanPwmPin, 255);
    } else {
      analogWrite(kFanPwmPin, 127);
    }
    //digitalWrite(kFanPin, on);
    //digitalWrite(kPumpPin, on);
    Serial.println(on);
  }*/
  /*FastLED.showColor(CRGB(0, 0, 255));
  delay(100);
  Serial.println(millis());*/

  //Serial.println(millis());
  //delay(500);
  //Serial.println(digitalRead(kNapButtonPin));
  //Serial.println(analogRead(kSpeedPin));

  /*Serial.print("Found ");
  Serial.print(intTemp.getDeviceCount(), DEC);
  Serial.println(" devices.");
   
  Serial.print("Parasite power is: "); 
  if (intTemp.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!intTemp.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
    Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();


  intTemp.requestTemperatures();
  Serial.println(intTemp.getTempFByIndex(0));
 
  delay(1000);*/
}
/*void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}*/
