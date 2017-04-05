#ifndef VECTORFIELD2D_H
#define VECTORFIELD2D_H

#include "Curl.hpp"
#include <glm/glm.hpp>
//#include "VectorField.hpp"
#include <vector>

class VectorField2D /*: public VectorField*/ {
private:
  glm::vec2 _start;
  glm::vec2 _end;

  int _size;

  std::vector<glm::vec2> _field;

public:
  VectorField2D(glm::vec2 start, glm::vec2 end);
  VectorField2D(glm::vec2 start, glm::vec2 end, std::vector<glm::vec2> field);

  VectorField2D& add(const VectorField2D&);
  VectorField2D& add(const float);

  VectorField2D& mult(const float);
  VectorField2D normalize() const;
  VectorField2D curl();

  int getSize() const;

  std::vector<glm::vec2> getField() const;
  
  int getWidth() const;
  int getHeight() const;

};
#endif
