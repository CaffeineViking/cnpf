#include "VectorField3D.hpp"
#include <iostream>
#include <cmath>

VectorField3D::VectorField3D(glm::vec3 start, glm::vec3 end) 
  : _start(start), _end(end) 
{
  for(int i = 0; i < getSize(); i++)
    _field.push_back(glm::vec3());
}

VectorField3D::VectorField3D(glm::vec3 start, glm::vec3 end, std::vector<glm::vec3> field)
  : VectorField3D(start, end) 
{
  if(getSize() != (int)field.size())
    std::cerr << "Error VectorField3D: Dimension mismatch" << std::endl;
  else
    _field = field;
}

VectorField3D& VectorField3D::add(const VectorField3D& other) 
{
  if(other.getWidth()  != getWidth()  || 
     other.getHeight() != getHeight() ||
     other.getDepth()  != getDepth()) 
    {
      std::cerr << "Error VectorField3D: dimension mismatch" << std::endl;
    } else {
    for(int i = 0; i < getSize(); i++)
      _field.at(i) += other.getField().at(i);    
  }
  return *this;
}


VectorField3D& VectorField3D::add(const float s)
{
  for(int i = 0; i < getSize(); i++)
    _field.at(i) += s; 
  return *this;
}

VectorField3D& VectorField3D::mult(const float s)
{
  for(int i = 0; i < getSize(); i++)
    _field.at(i) *= s; 
  return *this;
}

VectorField3D VectorField3D::normalize() const 
{
  std::vector<glm::vec3> newField;
  for(int i = 0; i < getSize(); i++)
    newField.push_back(glm::normalize(_field.at(i)));

  VectorField3D newVectorField(_start, _end, newField);
  return newVectorField;
}

VectorField3D VectorField3D::curl() 
{
  Curl c;
  float scl = 0.05;
  std::vector<glm::vec3> newField;
  for(int x = _start.x; x <= _end.x; x++)
    for(int y = _start.y; y <= _end.y; y++)
      for(int z = _start.z; z <= _end.z; z++)
	newField.push_back(c.getCurl(x*scl, y*scl, z*scl));
  
  VectorField3D newVectorField(_start, _end, newField);
  return newVectorField;
}

int VectorField3D::getSize() const 
{
  return getWidth() * getHeight() * getDepth();
}

std::vector<glm::vec3> VectorField3D::getField() const 
{
  return _field;
}


int VectorField3D::getWidth() const 
{
  return floor(std::abs(_end.x - _start.x) + 1);
}

int VectorField3D::getHeight() const 
{
  return floor(std::abs(_end.y - _start.y) + 1);
}

int VectorField3D::getDepth() const 
{
  return floor(std::abs(_end.z - _start.z) + 1);
}
