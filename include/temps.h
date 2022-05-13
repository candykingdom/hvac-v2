#ifndef TEMPS_H_
#define TEMPS_H_

class Temps {
 public:
  virtual bool Init();
  virtual float GetOutside();
  virtual float GetInside();
};

#endif  // TEMPS_H_