#ifndef VECTOR_FIELD_3D_H
#define VECTOR_FIELD_3D_H

#include "Curl.hpp"

#include <vector>
#include <glm/glm.hpp>

class VectorField3D {
private:
  glm::vec3 _start;
  glm::vec3 _end;
  std::vector<glm::vec3> _field;

public:
  VectorField3D(glm::vec3 start = glm::vec3(), glm::vec3 end = glm::vec3());
  VectorField3D(glm::vec3 start, glm::vec3 end, std::vector<glm::vec3> field);

  ~VectorField3D() = default;

  VectorField3D& add(const VectorField3D&);
  VectorField3D& add(const float);

  VectorField3D& mult(const float);
  VectorField3D normalize() const;
  VectorField3D curl();

  int getSize() const;

  std::vector<glm::vec3> getField() const;
  
  int getWidth() const;
  int getHeight() const;
  int getDepth() const;

};
#endif // VECTOR_FIELD_3D_H
