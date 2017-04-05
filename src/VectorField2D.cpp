#include "VectorField2D.hpp"
#include <iostream>
#include <cmath>


/* 
 * TODO:
 * Visualize vector field
 * Scale curl
 * 
 */
VectorField2D::VectorField2D(glm::vec2 start, glm::vec2 end) 
  : _start(start), _end(end) 
{
  for(int i = 0; i < getSize(); i++)
    _field.push_back(glm::vec2());
}

VectorField2D::VectorField2D(glm::vec2 start, glm::vec2 end, std::vector<glm::vec2> field)
  : VectorField2D(start, end) 
{
  if(getSize() != (int)field.size())
    std::cerr << "Error VectorField2D: Dimension mismatch" << std::endl;
  else
    _field = field;
}

VectorField2D& VectorField2D::add(const VectorField2D& other) 
{
  if(other.getWidth() != getWidth() || other.getHeight() != getHeight()) {
    std::cerr << "Error VectorField2D: dimension mismatch" << std::endl;
  } else {
    for(int i = 0; i < getSize(); i++)
      _field.at(i) += other.getField().at(i);    
  }
  return *this;
}


VectorField2D& VectorField2D::add(const float s)
{
  for(int i = 0; i < getSize(); i++)
    _field.at(i) += s; 
  return *this;
}

VectorField2D& VectorField2D::mult(const float s)
{
  for(int i = 0; i < getSize(); i++)
    _field.at(i) *= s; 
  return *this;
}

VectorField2D VectorField2D::normalize() const 
{
  std::vector<glm::vec2> newField;
  for(int i = 0; i < getSize(); i++)
    newField.push_back(glm::normalize(_field.at(i)));

  VectorField2D newVectorField(_start, _end, newField);
  return newVectorField;
}

VectorField2D VectorField2D::curl() 
{
  Curl c;
  float scl = 0.005;

  std::vector<glm::vec2> newField;
  for(int x = _start.x; x <= _end.x ; x++)
    for(int y = _start.y; y <= _end.y; y++)
      newField.push_back(c.getCurl(x*scl, y*scl));
  
  VectorField2D newVectorField(_start, _end, newField);
  return newVectorField;
}

int VectorField2D::getSize() const 
{
  return getWidth() * getHeight();
}

std::vector<glm::vec2> VectorField2D::getField() const 
{
  return _field;
}


int VectorField2D::getWidth() const 
{
  return floor(std::abs(_end.x - _start.x) + 1);
}

int VectorField2D::getHeight() const 
{
  return floor(std::abs(_end.y - _start.y) + 1);
}
