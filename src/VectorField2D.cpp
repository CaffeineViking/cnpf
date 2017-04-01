#include "VectorField2D.hpp"
#include <iostream>
#include <cmath>


/* 
 * TODO:
 * Visualize vector field
 * Scale curl
 * 
 */
VectorField2D::VectorField2D(glm::vec2 start, glm::vec2 end) {
  _start = start;
  _end = end;

  for(int i = 0; i < getSize(); i++)
    _field.push_back(glm::vec2());
}

VectorField2D::VectorField2D(glm::vec2 start, glm::vec2 end, std::vector<glm::vec2> field)
  : VectorField2D(start, end) {
  if(getSize() != (int)field.size())
    std::cerr << "Error VectorField2D: Dimension mismatch" << std::endl;
else
  _field = field;
}

VectorField* VectorField2D::add(VectorField* o) {
  VectorField2D* other = dynamic_cast<VectorField2D*>(o);
  if(!other) {
    std::cerr << "Error VectorField2D: Can only add VectorField2D" << std::endl;
    return this;
  }

  if(other->getWidth() != getWidth() || other->getHeight() != getHeight()) {
    std::cerr << "Error VectorField2D: dimension mismatch" << std::endl;
  } else {
    for(int i = 0; i < getSize(); i++)
      _field.at(i) += other->getField().at(i);    
  }
  return this;
}


VectorField* VectorField2D::add(float s){
  for(int i = 0; i < getSize(); i++)
    _field.at(i) += s; 
  return this;
}

VectorField* VectorField2D::mult(float s){
  for(int i = 0; i < getSize(); i++)
    _field.at(i) *= s; 
 return this;
}

VectorField* VectorField2D::normalize(){
  std::vector<glm::vec2> newField;
  for(int i = 0; i < getSize(); i++)
    newField.push_back(glm::normalize(_field.at(i)));
  return new VectorField2D(_start, _end, newField);
}


VectorField* VectorField2D::curl() {
  Curl c;
  std::vector<glm::vec2> newField;
  for(int x = _start.x; x <= _end.x; x++)
    for(int y = _start.y; y <= _end.y; y++)
      newField.push_back(c.getCurl(x, y));
  
  return new VectorField2D(_start, _end, newField);
}


int VectorField2D::getSize() const {
  return std::round(std::abs((_end.x - _start.x + 1)*(_end.y - _start.y + 1)));
}

std::vector<glm::vec2> VectorField2D::getField() const {
  return _field;
}


int VectorField2D::getWidth() const {
  return floor(_end.x - _start.x + 1);
}

int VectorField2D::getHeight() const {
  return floor(_end.y - _start.y + 1);
}
