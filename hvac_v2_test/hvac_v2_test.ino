#include <FastLED.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Bounce2.h>


const int kNumLeds = 3;
const int kButtonPin = 9;
const int kSpeedPin = A2;
const int kIntTempPin = 11;
const int kFanPin = 6;
const int kPumpPin = 7;

CRGB leds[kNumLeds];


OneWire oneWire(kIntTempPin);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

Bounce auxButton;

void setup() {
  // put your setup code here, to run once:
  //FastLED.addLeds<WS2812, 2>(leds, kNumLeds);
  //FastLED.showColor(CHSV(0, 255 ,255));

  Serial.begin(9600);
  //pinMode(kSpeedPin, INPUT);
  pinMode(kIntTempPin, INPUT_PULLUP);
  pinMode(kFanPin, OUTPUT);
  pinMode(kPumpPin, OUTPUT);

  sensors.begin();
  auxButton.attach(kButtonPin, INPUT_PULLUP);
}


bool on = false;
void loop() {

  if (auxButton.update() && auxButton.read()) {
    on = !on;
    digitalWrite(kFanPin, on);
    digitalWrite(kPumpPin, on);
    Serial.println(on);
  }
  // put your main code here, to run repeatedly:
  //FastLED.showColor(CHSV(0, 255 ,255));

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


void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
