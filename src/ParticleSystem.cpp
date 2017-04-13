#include "ParticleSystem.hpp"

#include <iostream>
#include <CL/cl.hpp>
#include <GL/glew.h>
#include <algorithm>
#include "VectorField3D.hpp"
#include <sstream>
#include "simplexnoise1234.hpp"
#include <glm/gtc/noise.hpp>
#ifdef WINDOWS
#include <windows.h>
#endif

void bufferToPNG(const std::string& name, const std::vector<float>& data, int width, int height, int depth){
      for(int x = 0; x < width; x++) {
      std::vector<float> crossSection(height * depth * 3);
      for(int y = 0; y < height; y++) {
         for(int z = 0; z < depth; z++) {
          int globalIndex = (x * height * depth) + (y * depth) + z;
          int subIndex = (y * depth) + z;
          crossSection[subIndex * 3 + 0] = data.at(globalIndex * 3 + 0);
          crossSection[subIndex * 3 + 1] = data.at(globalIndex * 3 + 1);
          crossSection[subIndex * 3 + 2] = data.at(globalIndex * 3 + 2);
        }
     }
       std::ostringstream oss;
       oss << name << (x + 1) << ".png";
       OpenGLUtils::writePNG(oss.str(), height, depth, crossSection);
   }
}

ParticleSystem::~ParticleSystem(){}

ParticleSystem::ParticleSystem(const int particles, const float time): PARTICLE_COUNT{particles}, maxTime{time}{
}


float ramp(float d){
      if( d >= 1.0f){
         return 1.0f;
      }
      else if( d < 1.0f && d > -1.0f){
         return (15.0f/8.0f * d) - (10.0f/3.0f * (d*d*d)) + (3.0f/8.0f * (d*d*d*d*d));
      }else{
         return -1.0f;
      }
}
// =====================================
// Setup devices and kernel for OpenCL  ( ͡° ͜ʖ ͡°).
// =====================================
bool ParticleSystem::init(const std::string& path, const std::string& kernel, const std::string& device, const ShaderProgram& program){
// Init random kernel with my NVIDIA card. You need to change
   std::cout << "Init OpenCL with device " << device <<  std::endl; 
   _params = OpenCLUtils::initCL(path, kernel, device);

// Load texture and place in GPU
      _width  = 64;
      _height = 64;
      _depth  = 64;

      // Construct 
      glm::vec3 center(_width/2.0f,_height/2.0f,_width/2.0f);
      float maskRadius = glm::distance(center, glm::vec3(0.0f,0.0f,0.0f)) / 3.0f;
   VectorField3D obstacle(_width,_height,_depth);

   // Calculate distance field
   for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {
            int index = (x * _height * _depth) + (y * _depth) + z;
            float d = glm::distance(glm::vec3(x,y,z), center);
            if(d < maskRadius)
               obstacle[index] = glm::vec3(0.0f);
            else
               obstacle[index] = glm::vec3(1.0f);
         }
      }
   }
   bufferToPNG("debug/obstacle",  (obstacle.normalize() * 0.5f + 0.5f).getData() , _width, _height, _depth);

   VectorField3D distanceField(_width,_height,_depth);
   for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {
            int index = (x * _height * _depth) + (y * _depth) + z;
            float d = glm::distance(glm::vec3(x,y,z), center);
            if(d < maskRadius)
               distanceField[index] = glm::vec3(0.0f);
            else
               distanceField[index] = glm::vec3(d) - maskRadius;
         }
      }
   }
   distanceField = distanceField.selfNormalize();
   bufferToPNG("debug/distanceField", distanceField.getData() , _width, _height, _depth);

   VectorField3D distanceGradient = distanceField.gradient().normalize();

   // Callculate distance gradient

   bufferToPNG("debug/distanceGradient", (distanceGradient * 0.5f + 0.5f).getData() , _width, _height, _depth);

   VectorField3D normalField(_width,_height,_depth);
   for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {
            glm::vec3 position = glm::vec3(x,y,z);
            glm::vec3 fieldValue = distanceField.get(position);
            
            float distance = glm::distance(position, center);
            float alpha = glm::abs(ramp(distance / ((float)_width)));
            glm::vec3 normal =  glm::normalize(position - center);

            int index = (x * _height * _depth) + (y * _depth) + z;
            normalField[index] = alpha * fieldValue + (1-alpha)*normal*(glm::dot(normal,fieldValue));
         }
      }
   }
   bufferToPNG("debug/normalField", (normalField.normalize() * 0.5f + 0.5f).getData() , _width, _height, _depth);

   VectorField3D noiseField(_width,_height,_depth);

    for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {

            float noise1 = glm::simplex(glm::vec3(x * 0.01f + 10.0f,y *  0.01f + 10.0f,z *  0.01f + 10.0f));
            float noise2 = glm::simplex(glm::vec3(x * 0.03f,y *  0.03f,z *  0.03f));
            float noise3 = glm::simplex(glm::vec3(x * 0.06f,y *  0.06f,z *  0.06f));
            float noise = noise1 + noise2 + noise3;
            int index = (x * _height * _depth) + (y * _depth) + z;
            noiseField[index] =glm::vec3(noise,noise,noise);
         }
      }
   }

   noiseField = noiseField.selfNormalize() * distanceField;
   bufferToPNG("debug/noiseField", (noiseField.selfNormalize() * 0.5f + 0.5f).getData() , _width, _height, _depth);

   VectorField3D potentialField = normalField;
   bufferToPNG("debug/potentialField", (potentialField.selfNormalize() * 0.5f + 0.5f).getData() , _width, _height, _depth);

   VectorField3D curlField(_width, _height, _depth);
   // Callculate curl
    for(int x = 0; x < _width; x++) {
      for(int y = 0; y < _height; y++) {
         for(int z = 0; z < _depth; z++) {
           
           float eps = 0.1f;
           float n1,n2,a,b;
           glm::vec3 curl(0.0f);

           n1 = potentialField.get(glm::vec3(x,y + eps,z)).y;
           n2 = potentialField.get(glm::vec3(x,y - eps,z)).y;
           a = (n1 - n2) / (2.0f * eps);

           n1 = potentialField.get(glm::vec3(x,y,z + eps)).z;
           n2 = potentialField.get(glm::vec3(x,y,z - eps)).z;
           b = (n1 - n2) / (2.0f * eps);

           curl.x = a - b;

           n1 = potentialField.get(glm::vec3(x,y,z + eps)).z;
           n2 = potentialField.get(glm::vec3(x,y,z - eps)).z;
           a = (n1 - n2) / (2.0f * eps);

           n1 = potentialField.get(glm::vec3(x + eps,y,z)).x;
           n2 = potentialField.get(glm::vec3(x - eps,y,z)).x;
           b = (n1 - n2) / (2.0f * eps);

           curl.y = a - b;

           n1 = potentialField.get(glm::vec3(x + eps,y,z)).x;
           n2 = potentialField.get(glm::vec3(x - eps,y,z)).x;
           a = (n1 - n2) / (2.0f * eps);

           n1 = potentialField.get(glm::vec3(x,y + eps,z)).y;
           n2 = potentialField.get(glm::vec3(x,y - eps,z)).y;
           b = (n1 - n2) / (2.0f * eps);

           curl.z = a - b;

           int index = (x * _height * _depth) + (y * _depth) + z;
           curlField[index] = glm::normalize(curl);
            //std::cout << x << " " << y << " " << z << " : " << curlField.at(index).x << " " << curlField.at(index).y << " " << curlField.at(index).z << std::endl;
         }
      }
   }

   std::vector<float> textureData = (curlField * 0.5f + 0.5f).getData();

   bufferToPNG("debug/curl", textureData , _width, _height, _depth);


   GLuint glTexture = OpenGLUtils::createTexture3D(_width, _height,_depth, textureData.data());
   int errCode;
   _texture = cl::ImageGL(_params.context,CL_MEM_READ_ONLY,GL_TEXTURE_3D,0,glTexture,&errCode);
   if (errCode!=CL_SUCCESS) {
      std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
      return 250;
    }

   std::default_random_engine generator{};
   std::uniform_int_distribution<int> distribution_int(0,_emitters.size()-1);
   std::uniform_real_distribution<float> distribution_float(0.0f,1.0f);
   std::vector<float> data(3*PARTICLE_COUNT);
   for(int n = 0; n < PARTICLE_COUNT; ) {
   	auto emitter = _emitters.at(distribution_int(generator));
   	glm::vec3 min = emitter.first - (emitter.second/2.0f);
   	glm::vec3 max = emitter.first + (emitter.second/2.0f);
      float x = min.x + std::abs(max.x - min.x)*distribution_float(generator);
      float y = min.y + std::abs(max.y - min.y)*distribution_float(generator);
      float z = min.z + std::abs(max.z - min.z)*distribution_float(generator);
      if(glm::length(glm::vec3(x,y,z)) > maskRadius+4.0f){
         data[3*n+0] = x;
         data[3*n+1] = y;
         data[3*n+2] = z;
         ++n; 
      }
   }

// Same but for OpenGL
   GLuint vbo = OpenGLUtils::createBuffer(3*PARTICLE_COUNT, data.data(), GL_DYNAMIC_DRAW);
// _tmp buffer to do some copying on
   _tmp = cl::BufferGL(_params.context, CL_MEM_READ_WRITE, vbo, NULL);

// Create Vertex buffer for the positions
   _vertexBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*PARTICLE_COUNT);
// Write position data to buffer
   _params.queue.enqueueWriteBuffer(_vertexBuffer, CL_TRUE, 0, sizeof(float)*3*PARTICLE_COUNT, data.data());
// Bind buffer to shader
   glBindBuffer(GL_ARRAY_BUFFER,vbo);
   GLint position_attribute = glGetAttribLocation(program.getId(), "position");
   glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(position_attribute);
   glBindVertexArray(0);
    std::normal_distribution<float> distribution(1.0f,0.2f);
// Create velocity buffer, this is not of interest to the renderer at the moment
   for(int n = 0; n < PARTICLE_COUNT; ++n) {
      data[3*n+0] = distribution(generator);
      data[3*n+1] = distribution(generator);
      data[3*n+2] = distribution(generator);
   }
// Create Vertex buffer for the velocities
   _velocityBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*PARTICLE_COUNT);
// Write velocities data to buffer
   _params.queue.enqueueWriteBuffer(_velocityBuffer, CL_TRUE, 0, sizeof(float)*3*PARTICLE_COUNT, data.data());
   return true;
}

void ParticleSystem::addEmitter(const glm::vec3& position, const glm::vec3& dimensions){
	_emitters.push_back(std::make_pair(position, dimensions));
}

void ParticleSystem::compute(const float time, const float timeDelta){
// CL event used to wait for kernel osv...
   cl::Event ev;
// set kernel arguments
//======================================================
// Vertex array object buffer with coords interleaved
//======================================================
// Wait for kernel
   glFinish();

   std::vector<cl::Memory> objs;
   objs.clear();
   objs.push_back(_tmp);
   objs.push_back(_texture);
// Aquire GL Object ( ͡° ͜ʖ ͡°)
   cl_int res = _params.queue.enqueueAcquireGLObjects(&objs,NULL,&ev);
   ev.wait();

   if (res!=CL_SUCCESS) {
      std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
      return;
   }

   _params.kernel.setArg(0,_vertexBuffer);
   _params.kernel.setArg(1,_velocityBuffer);
   _params.kernel.setArg(2,_texture);
   _params.kernel.setArg(3,_width);
   _params.kernel.setArg(4,_height);
   _params.kernel.setArg(5,_depth);
   _params.kernel.setArg(6,timeDelta);

// Equeue kernel
   _params.queue.enqueueNDRangeKernel(_params.kernel,cl::NullRange,cl::NDRange(getParticleCount(time)),cl::NDRange(1));

// Copy from OpenCL to OpenGL 
   _params.queue.enqueueCopyBuffer(_vertexBuffer, _tmp, 0, 0, 3*PARTICLE_COUNT*sizeof(float), NULL, NULL);
   res = _params.queue.enqueueReleaseGLObjects(&objs);
   ev.wait();
   if (res!=CL_SUCCESS) {
      std::cout<<"Failed releasing GL object: "<<res<<std::endl;
      return;
   }

// Wait for copy to be done
   _params.queue.finish();
}

int ParticleSystem::getParticleCount(const float time) const {
   return std::min(time/maxTime,1.0f)*PARTICLE_COUNT;
}

