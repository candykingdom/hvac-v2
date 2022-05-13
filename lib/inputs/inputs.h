#ifndef INPUTS_H_
#define INPUTS_H_

class Inputs {
 public:
  virtual bool Init();
  virtual float GetOutside();
  virtual float GetInside();
  virtual bool GetWaterSwitch();
};

#endif  // INPUTS_H_