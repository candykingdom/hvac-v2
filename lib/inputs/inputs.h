#ifndef INPUTS_H_
#define INPUTS_H_

class Inputs {
 public:
  virtual bool Init();
  virtual float GetOutside();
  virtual float GetInside();
};

#endif  // INPUTS_H_