#include "Curl.hpp"

glm::vec2 Curl::getCurl(glm::vec2 v) {
  float n1, n2, x, y;

  n1 = snoise2(v.x, v.y + _eps);
  n2 = snoise2(v.x, v.y - _eps);
  x = (n1-n2)/(2*_eps);

  n1 = snoise2(v.x + _eps, v.y);
  n2 = snoise2(v.x - _eps, v.y);
  y = (n1-n2)/(2*_eps);

  return glm::vec2(x, -y);
}

glm::vec3 Curl::getCurl(glm::vec3 v) {
  float n1, n2;
  float dx, dy, dz;
  float x, y, z;

  n1 = snoise3(v.x + _eps, v.y, v.z);
  n2 = snoise3(v.x - _eps, v.y, v.z);
  dx = (n1-n2)/(2*_eps);
    
  n1 = snoise3(v.x, v.y + _eps, v.z);
  n2 = snoise3(v.x, v.y - _eps, v.z);
  dy = (n1-n2)/(2*_eps);

  n1 = snoise3(v.x, v.y, v.z + _eps);
  n2 = snoise3(v.x, v.y, v.z - _eps);
  dz = (n1-n2)/(2*_eps);

  // x = dz1/dy - dy1/dz
  x = dy-dz;

  // y = dx1/dz - dz1/dx
  y = dz-dx;

  // z = dy1/dx - dx1/dy
  z = dx-dy;

  return glm::vec3(x, y, z);
}

glm::vec3 Curl::getCurl(glm::vec3 v, float t) {
  float n1, n2;
  float dx, dy, dz;
  float x, y, z;

  n1 = snoise4(v.x + _eps, v.y, v.z, t);
  n2 = snoise4(v.x - _eps, v.y, v.z, t);
  dx = (n1-n2)/(2*_eps);
    
  n1 = snoise4(v.x, v.y + _eps, v.z, t);
  n2 = snoise4(v.x, v.y - _eps, v.z, t);
  dy = (n1-n2)/(2*_eps);

  n1 = snoise4(v.x, v.y, v.z + _eps, t);
  n2 = snoise4(v.x, v.y, v.z - _eps, t);
  dz = (n1-n2)/(2*_eps);

  // x = dz1/dy - dy1/dz
  x = dy-dz;

  // y = dx1/dz - dz1/dx
  y = dz-dx;

  // z = dy1/dx - dx1/dy
  z = dx-dy;

  return glm::vec3(x, y, z);
}

glm::vec2 Curl::getCurl(float x, float y) {
  return getCurl(glm::vec2(x, y));
}

glm::vec3 Curl::getCurl(float x, float y, float z){
  return getCurl(glm::vec3(x, y, z));
}

glm::vec3 Curl::getCurl(float x, float y, float z, float t) {
  return getCurl(glm::vec3(x, y, z), t);
}


void Curl::setDiff(float e) {
  _eps = e;
}

float Curl::getDiff() const {
  return _eps;
}
