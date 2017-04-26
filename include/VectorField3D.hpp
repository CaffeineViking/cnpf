#ifndef VECTOR_FIELD_3D_H
#define VECTOR_FIELD_3D_H

#include <vector>
#include <glm/glm.hpp>

class VectorField3D {
private:
  int _width, _height, _depth;
  std::vector<glm::vec3> _field;

public:
  VectorField3D(int, int, int);
  // VectorField3D(glm::vec3 start, glm::vec3 end, std::vector<glm::vec3> field);

  VectorField3D(const VectorField3D&);
  VectorField3D& operator=(const VectorField3D&);

  VectorField3D(VectorField3D&&);
  VectorField3D& operator=(VectorField3D&&);

  ~VectorField3D() = default;

  VectorField3D normalize();
  VectorField3D selfNormalize();
  VectorField3D operator/ (const VectorField3D&);
  VectorField3D operator/ (const float);

  VectorField3D operator- (const VectorField3D&);
  VectorField3D operator- (const float);

  VectorField3D operator* (const VectorField3D&);
  VectorField3D operator* (const float);

  VectorField3D operator+ (const VectorField3D&);
  VectorField3D operator+ (const glm::vec3&);
  VectorField3D operator+ (const float);

  VectorField3D gradient();
  VectorField3D curl(const float);

  glm::vec3& operator[] (const int);

  glm::vec3 get(const glm::vec3&);
  glm::vec3 at(const int) const;
  const std::vector<glm::vec3>& getField() const;
  std::vector<float> getData() const;
  int getWidth()  const;
  int getHeight() const;
  int getDepth()  const;

};
#endif // VECTOR_FIELD_3D_H
