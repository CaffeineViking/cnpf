#ifndef CURL_HPP
#define CURL_HPP

#include "simplexnoise1234.hpp"
#include <glm/glm.hpp>

class Curl {
private:
  float _eps = 0.0001;

public:
  Curl() = default;
  ~Curl() = default;

  glm::vec2 getCurl(glm::vec2 v);
  glm::vec3 getCurl(glm::vec3 v);
  glm::vec3 getCurl(glm::vec3, float t);

  glm::vec2 getCurl(float x, float y);
  glm::vec3 getCurl(float x, float y, float z);
  glm::vec3 getCurl(float x, float y, float z, float t);

  void setDiff(float e); 
  float getDiff() const;
};
#endif
