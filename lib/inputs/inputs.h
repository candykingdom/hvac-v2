#ifndef INPUTS_H_
#define INPUTS_H_

class Inputs {
 public:
  virtual bool Init() = 0;
  virtual float GetOutside() = 0;
  virtual float GetInside() = 0;
  virtual bool GetWaterSwitch() = 0;
  virtual uint16_t GetBatteryMillivolts() = 0;

  static constexpr float kNoTemp = -459.67;
};

#endif  // INPUTS_H_