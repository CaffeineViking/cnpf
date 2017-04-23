#include "VectorField3D.hpp"
#include <iostream>
#include <cmath>
#include <utility>
#include <iterator>
#include <algorithm>
#include <exception>
#include <glm/glm.hpp>
VectorField3D::VectorField3D(int width, int height, int depth): 
_width{width},
_height{height},
_depth{depth}
{
  _field = std::vector<glm::vec3>(width*height*depth);
}

VectorField3D::VectorField3D(const VectorField3D& f){
  _width = f.getWidth();
  _height = f.getHeight();
  _depth = f.getDepth();
  _field = f.getField();
}

VectorField3D& VectorField3D::operator=(const VectorField3D& f){
  _width = f.getWidth();
  _height = f.getHeight();
  _depth = f.getDepth();
  _field = f.getField();
  return *this;
}

VectorField3D::VectorField3D(VectorField3D&& f){
  _width = std::move(f.getWidth());
  _height = std::move(f.getHeight());
  _depth = std::move(f.getDepth());
  _field = std::move(f.getField());
}

VectorField3D& VectorField3D::operator=(VectorField3D&& f){
  _width = std::move(f.getWidth());
  _height = std::move(f.getHeight());
  _depth = std::move(f.getDepth());
  _field = std::move(f.getField());
  return *this;

}

VectorField3D VectorField3D::normalize() {
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth; i++) {

    glm::vec3 r = glm::normalize(_field.at(i));
    result[i] = r;
//    std::cout << r.x << " " << r.y << " " << r.z << std::endl;
  }
  return result;
}

VectorField3D VectorField3D::selfNormalize() {
  VectorField3D result(_width,_height,_depth);
  glm::vec3 max = *(std::max_element(std::begin(_field),std::end(_field), ([](const glm::vec3& x, const glm::vec3& y)
  {
    float a = glm::length(x);
    float b = glm::length(y);
    return a < b ;
  })));
  for(int i = 0; i < _width*_height*_depth; i++) {

    glm::vec3 r = _field.at(i) / max;
    result[i] = r;
  }
  return result;
}

VectorField3D VectorField3D::gradient() {
  VectorField3D result(_width,_height,_depth);
  for(int x = 0; x < _width; x++) {
    for(int y = 0; y < _height; y++) {
      for(int z = 0; z < _depth; z++) {
        const glm::vec3 position = glm::vec3(x,y,z);
        const float eps = 0.01f;
        const glm::vec3 dx(eps, 0.0f, 0.0f);
        const glm::vec3 dy(0.0f, eps, 0.0f);
        const glm::vec3 dz(0.0f, 0.0f, eps);

        glm::vec3 d = get(position);

//glm::vec3 sign = glm::sign(glm::vec3(x,y,z) - glm::vec3(_width/2.0f, _height/2.0f, _depth/2.0f));
        float dfdx = (get(position + dx).x - d.x);
        float dfdy = (get(position + dy).y - d.y);
        float dfdz = (get(position + dz).z - d.z);
//std::cout << dfdx << " " << dfdy << " " << dfdz << std::endl;
        int index = (x * _height * _depth) + (y * _depth) + z;
        result[index] = glm::vec3(dfdx,dfdy,dfdz);
//std::cout << field.at(index).x << " " << field.at(index).y << " " << field.at(index).z << std::endl;
      }
    }
  }
  return result;
}

VectorField3D VectorField3D::curl(const float eps = 0.01f){
  VectorField3D result(_width, _height, _depth);
  for(int x = 0; x < _width; x++) {
    for(int y = 0; y < _height; y++) {
      for(int z = 0; z < _depth; z++) {
        float n1,n2,a,b;
        glm::vec3 curl(0.0f);

        n1 = get(glm::vec3(x,y + eps,z)).y;
        n2 = get(glm::vec3(x,y - eps,z)).y;
        a = (n1 - n2) / (2.0f * eps);

        n1 = get(glm::vec3(x,y,z + eps)).z;
        n2 = get(glm::vec3(x,y,z - eps)).z;
        b = (n1 - n2) / (2.0f * eps);

        curl.x = a - b;

        n1 = get(glm::vec3(x,y,z + eps)).z;
        n2 = get(glm::vec3(x,y,z - eps)).z;
        a = (n1 - n2) / (2.0f * eps);

        n1 = get(glm::vec3(x + eps,y,z)).x;
        n2 = get(glm::vec3(x - eps,y,z)).x;
        b = (n1 - n2) / (2.0f * eps);

        curl.y = a - b;

        n1 = get(glm::vec3(x + eps,y,z)).x;
        n2 = get(glm::vec3(x - eps,y,z)).x;
        a = (n1 - n2) / (2.0f * eps);

        n1 = get(glm::vec3(x,y + eps,z)).y;
        n2 = get(glm::vec3(x,y - eps,z)).y;
        b = (n1 - n2) / (2.0f * eps);

        curl.z = a - b;

        int index = (x * _height * _depth) + (y * _depth) + z;
        result[index] = curl;
      }
    }
  }
  return result;
}

VectorField3D VectorField3D::operator/ (const VectorField3D& f){
  if(f.getWidth() != _width || f.getHeight() != _height || f.getDepth() != _depth) {
    std::cerr << "Missmatch between field sizes" << std::endl;
    throw std::exception();
  }
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) / f.at(i);
  }
  return result;
}

VectorField3D VectorField3D::operator/ (const float f){
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) / f;
  }
  return result;
}

VectorField3D VectorField3D::operator- (const VectorField3D& f){
  if(f.getWidth() != _width || f.getHeight() != _height || f.getDepth() != _depth) {
    std::cerr << "Missmatch between field sizes" << std::endl;
    throw std::exception();
  }
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) - f.at(i);
  }
  return result;
}

VectorField3D VectorField3D::operator- (const float f){
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) - f;
  }
  return result;
}

VectorField3D VectorField3D::operator* (const VectorField3D& f){
  if(f.getWidth() != _width || f.getHeight() != _height || f.getDepth() != _depth) {
    std::cerr << "Missmatch between field sizes" << std::endl;
    throw std::exception();
  }
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) * f.at(i);
  }
  return result;
}

VectorField3D VectorField3D::operator* (const float f){
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) * f;
  }
  return result;
}

VectorField3D VectorField3D::operator+ (const glm::vec3& v){
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) + v;
  }
  return result;
}

VectorField3D VectorField3D::operator+ (const VectorField3D& f){
  if(f.getWidth() != _width || f.getHeight() != _height || f.getDepth() != _depth) {
    std::cerr << "Missmatch between field sizes" << std::endl;
    throw std::exception();
  }
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) + f.at(i);
  }
  return result;
}
VectorField3D VectorField3D::operator+ (const float f){
  VectorField3D result(_width,_height,_depth);
  for(int i = 0; i < _width*_height*_depth;i++) {
    result[i] = _field.at(i) + f;
  }
  return result;
}


int toIndex(int x, int y, int z, int height, int depth){
  return (x * height * depth) + (y * depth) + z;
}

glm::vec3 VectorField3D::get(const glm::vec3& position){

// Extract sub-index distance
  int x = ((int)position.x); 
  int y = ((int)position.y); 
  int z = ((int)position.z);
  int index = toIndex(x, y, z, _height, _depth);
  glm::vec3 eps = position - glm::vec3(x,y,z);

// Find the closest index to interpolate with
  int xStep = glm::sign(eps.x);
  int yStep = glm::sign(eps.y);
  int zStep = glm::sign(eps.z);
//std::cout << xStep << " " << yStep << " " << zStep << std::endl;

// Calculate the new index so it stays in bounds
  int xOffset = glm::clamp(x + xStep, 0, _width  - 1);
  int yOffset = glm::clamp(y + yStep, 0, _height - 1);
  int zOffset = glm::clamp(z + zStep, 0, _depth  - 1);
//std::cout << xOffset << " " << yOffset << " " << xOffset << std::endl;

// Get the value in the new index
  glm::vec3 xValueOffset = _field.at(toIndex(xOffset, y       , z       , _height, _depth));
  glm::vec3 yValueOffset = _field.at(toIndex(x      , yOffset , z       , _height, _depth));
  glm::vec3 zValueOffset = _field.at(toIndex(x      , y       , zOffset , _height, _depth));
//std::cout << xs << " " << ys << " " << zs << std::endl;

// Calculate output as the linear interpolation betwwen the old index and the new index
  glm::vec3 c = _field.at(index);
  float dx = glm::mix(c.x, xValueOffset.x, eps.x);
  float dy = glm::mix(c.y, yValueOffset.y, eps.y);
  float dz = glm::mix(c.z, zValueOffset.z, eps.z);

  return glm::vec3(dx,dy,dz);
}

glm::vec3 VectorField3D::at(const int index) const {
  return _field.at(index);
}

glm::vec3& VectorField3D::operator[] (const int index) {
  return _field.at(index);  
}


const std::vector<glm::vec3>& VectorField3D::getField() const {
  return _field;
}

std::vector<float> VectorField3D::getData() const{
  std::vector<float> data(_width*_height*_depth*3);
  for(int i = 0; i < _width*_height*_depth; i++){
    data[i*3 + 0] = _field.at(i).x;
    data[i*3 + 1] = _field.at(i).y;
    data[i*3 + 2] = _field.at(i).z;
  }
  return data;
}

int VectorField3D::getWidth() const {
  return _width;
}

int VectorField3D::getHeight() const {
  return _height;
}

int VectorField3D::getDepth() const {
  return _depth;
}
