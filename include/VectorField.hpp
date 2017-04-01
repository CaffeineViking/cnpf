#ifndef VECTORFIELD_H
#define VECTORFIELD_H

#include "Curl.hpp"

class VectorField {
private:

public:
  VectorField() = default;
  virtual ~VectorField() = default;

  virtual VectorField* add(VectorField*) = 0;
  virtual VectorField* add(float) = 0;

  virtual VectorField* mult(float) = 0;
  virtual VectorField* normalize() = 0;
  virtual VectorField* curl() = 0;
};
#endif
