#include "Scenario.hpp"
#include <glm/glm.hpp>
#include "OpenGLUtils.hpp"
#include "OpenCLUtils.hpp"
#include "Noise.hpp"
#include <iostream>
Scenario::Scenario(int w,int h,int d): _width{w}, _height{h}, _depth{d}, _field{w,h,d}
{}

int Scenario::getWidth() const{
	return _width;
}

int Scenario::getHeight() const{
	return _height;
}

int Scenario::getDepth() const{
	return _depth;
}

VectorField3D Scenario::getField() const{
	return _field;
}


// =======================
// Scenarios beneath
// =======================

// Debug scenario to verify field
DebugScenario::DebugScenario(int w,int h,int d): Scenario(w,h,d)
{}

glm::vec3 DebugScenario::potential(const glm::vec3&, const Noise&){

return glm::vec3(0.0f);
}

void DebugScenario::generate(){
	VectorField3D noiseField(_width,_height,_depth);
    for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {
            int index = (x * _height * _depth) + (y * _depth) + z;
            noiseField[index] = glm::vec3(0.0f,1.0f,0.0f);
         }
      }
   }
      
   _field = (noiseField * 0.5f) + 0.5f;
}

// Backwake scenario in 2D
BackwakeScenario::BackwakeScenario(int w,int h,int d): Scenario(w,h,d)
{}


glm::vec3 BackwakeScenario::potential(const glm::vec3& position, const Noise& noise){

   return glm::vec3(0.0f);
}

void BackwakeScenario::generate(){


   VectorField3D noiseField(_width,_height,_depth);

   Noise noise;
   noise.addNoise(glm::vec3(10.0f), glm::vec3(0.01f));
   noise.addNoise(glm::vec3(0.0f),  glm::vec3(0.13f));
   noise.addNoise(glm::vec3(0.0f),  glm::vec3(0.19f));

    for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {
            glm::vec3 position(x,y,z);

            int index = (x * _height * _depth) + (y * _depth) + z;
            float n = noise.get(position);
            noiseField[index] = glm::vec3(n,n,n);
         }
      }
   }
   noiseField = (noiseField.selfNormalize() * 0.5f) + 0.5f;

   OpenGLUtils::bufferToPNG_Y("debug/noiseFieldY", noiseField.getData() , _width, _height, _depth);
   //OpenGLUtils::bufferToPNG_X("debug/noiseFieldX", (noiseField* 0.5f + 0.5f).getData() , _width, _height, _depth);
 
   _field = noiseField;

   std::cout << "Field generated for BackwakeScenario" << std::endl;
}
