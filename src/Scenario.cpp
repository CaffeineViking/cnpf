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


// Helper functions ripped from the original paper.
template<class T>
inline T smooth_step(T r)
{
   if(r<0) return 0;
   else if(r>1) return 1;
   // You notice an academic paper when the polynomials are factorized
   return r*r*r*(10+r*(-15+r*6));
}

template<class T>
inline T smooth_step(T r, T r_lower, T r_upper, T value_lower, T value_upper)
{ return value_lower + smooth_step((r-r_lower)/(r_upper-r_lower)) * (value_upper-value_lower); }

template<class T>
inline T ramp(T r)
{ return smooth_step((r+1)/2)*2-1; }

float solid_distance(const glm::vec3& a, const glm::vec3& centre, const float radius)
{
   return glm::distance(a,centre) - radius;
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
   const glm::vec3   disc_centre(_width/2.0f,0.0f,_depth/2.0f);
   const float       disc_radius = 8.2f;
   const float       disc_influence = 8.2f;
   const float       background_flow_speed = -1.4f;
   const float       wake_expansion = 10.2f;

   float p = -background_flow_speed * position.z - disc_centre.z;
   float d = solid_distance(position, disc_centre, disc_radius);
   if(d < disc_influence){
      p *= ramp(d/disc_influence);
   }
   float wake_x = smooth_step(_width - (position.x - disc_centre.x)/disc_radius);
   if(wake_x > 0){
      float wake_z = smooth_step(_depth - glm::abs(position.z - disc_centre.z)/
         (1.5f*disc_radius+wake_expansion*(disc_centre.x - position.x)));
      if(wake_z > 0){
         float wake = wake_x*wake_z;
         float n = noise.get((position - glm::vec3(-background_flow_speed,0.0f,0.0f)) )* 0.6f ;
         float s = ramp(d) * n;
         p += wake * s;
      }
   }
   return glm::vec3(p,0,p);
}

void BackwakeScenario::generate(){


   VectorField3D noiseField(_width,_height,_depth);

   Noise noise;
   noise.addNoise(glm::vec3(10.0f), glm::vec3(0.01f));
   noise.addNoise(glm::vec3(0.0f),  glm::vec3(0.03f));
   noise.addNoise(glm::vec3(0.0f),  glm::vec3(0.09f));

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
