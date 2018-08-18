#include <FastLED.h>

// https://github.com/PaulStoffregen/OneWire
#include <OneWire.h>
// https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DallasTemperature.h>
// https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>


const int kNumLeds = 3;
const int kButtonPin = 9;
const int kSpeedPin = A2;
const int kIntTempPin = 12;
const int kFanPin = 6;
const int kPumpPin = 7;
const int kFanPwmPin = 8;

CRGB leds[kNumLeds];


OneWire oneWire(kIntTempPin);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

Bounce auxButton;

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  FastLED.addLeds<SK6812, 2>(leds, kNumLeds);
  FastLED.showColor(CHSV(0, 255 ,255));

  Serial.begin(9600);
  //pinMode(kSpeedPin, INPUT);
  pinMode(kIntTempPin, INPUT_PULLUP);
  pinMode(kFanPin, OUTPUT);
  pinMode(kPumpPin, OUTPUT);
  pinMode(kFanPwmPin, OUTPUT);

  sensors.begin();
  auxButton.attach(kButtonPin, INPUT_PULLUP);

  digitalWrite(kFanPin, HIGH);
  digitalWrite(kPumpPin, HIGH);
  analogWrite(kFanPwmPin, 255);
}


bool on = true;
void loop() {
  /*digitalWrite(kFanPwmPin, HIGH);
  delay(10);
  digitalWrite(kFanPwmPin, LOW);
  delay(10);
  analogWrite(kFanPwmPin, 10);
  digitalWrite(kFanPwmPin, HIGH);
  delay(100);
  digitalWrite(kFanPwmPin, LOW);
  delay(100);*/

  /*if (auxButton.update() && auxButton.read()) {
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
  // put your main code here, to run repeatedly:
  FastLED.showColor(CRGB(255, 255, 255));
  delay(100);
  Serial.println(millis());

  //Serial.println(millis());
  //delay(500);
  //Serial.println(digitalRead(kButtonPin));
  //Serial.println(analogRead(kSpeedPin));

  /*Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
   
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
    Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();


  sensors.requestTemperatures();
  Serial.println(sensors.getTempFByIndex(0));
 
  delay(1000);*/
}
