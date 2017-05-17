#include "ParticleSystem.hpp"

#include <random>
#include <iostream>
#include <GL/glew.h>
#include <CL/cl.hpp>
#include <algorithm>
#include "VectorField3D.hpp"
#include "simplexnoise1234.hpp"
#include <limits>
#include "Noise.hpp"
#include <algorithm>
#ifdef WINDOWS
#include <windows.h>
#endif

ParticleSystem::~ParticleSystem(){
    // Don't forget to clean this up!
    glDeleteBuffers(1, &_vertexBufferId);
}

ParticleSystem::ParticleSystem(const int particles, const float time): PARTICLE_COUNT{particles}, maxTime{time}{
}


// =====================================
// Setup devices and kernel for OpenCL  ( ͡° ͜ʖ ͡°).
// =====================================
bool ParticleSystem::init(std::vector<std::string> paths, std::vector<std::string> kernels, const std::string& device, const ShaderProgram& program){
// Init random kernel with my NVIDIA card. You need to change
   std::cout << "Init OpenCL with device " << device <<  std::endl; 
   _params = OpenCLUtils::initCL(paths, kernels, device);

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

      if(glm::length(glm::vec3(x,y,z)) < 0.0f)
         continue;
      data[3*n+0] = x;
      data[3*n+1] = y;
      data[3*n+2] = z;
      ++n;
   }

   _positionsBufferSize = 64;
   _positionsBufferHead = 0;

   std::vector<float> positionsBufferData(4*3*PARTICLE_COUNT);
   for(int i = 0; i < 4; i++)
       std::copy(data.begin(), data.end(), positionsBufferData.begin() + i*data.size());
   

   // Same but for OpenGL
   _vertexBufferId = OpenGLUtils::createBuffer(3*PARTICLE_COUNT, data.data(), GL_DYNAMIC_DRAW);
// _tmp buffer to do some copying on
   _tmp = cl::BufferGL(_params.context, CL_MEM_READ_WRITE, _vertexBufferId, NULL);
   
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++){
       _positionsBufferId[i] = OpenGLUtils::createBuffer(3*PARTICLE_COUNT, data.data(), GL_DYNAMIC_DRAW);
       _positionsGLBuffer[i] = cl::BufferGL(_params.context, CL_MEM_READ_WRITE, _positionsBufferId[i], NULL);
   }

// Create Vertex buffer for the positions
   _vertexBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*PARTICLE_COUNT);
   _positionsBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*PARTICLE_COUNT*_positionsBufferSize);

   for(int i = 0; i < _positionsBufferSize; i++){
       _params.queue.enqueueWriteBuffer(_positionsBuffer, CL_TRUE, i*sizeof(float)*3*PARTICLE_COUNT, sizeof(float)*3*PARTICLE_COUNT, data.data());
   }
   // Write position data to buffer
   _params.queue.enqueueWriteBuffer(_vertexBuffer, CL_TRUE, 0, sizeof(float)*3*PARTICLE_COUNT, data.data());
// Bind buffer to shader
   glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
   GLint position_attribute = glGetAttribLocation(program.getId(), "position");
   glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(position_attribute);

   GLint oldPosition_attribute = glGetAttribLocation(program.getId(), "positionsBuffer");
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++){
       glBindBuffer(GL_ARRAY_BUFFER, _positionsBufferId[i]);
       glVertexAttribPointer(oldPosition_attribute + i, 3, GL_FLOAT, GL_FALSE, 0, 0);
       glEnableVertexAttribArray(oldPosition_attribute + i);
   }
   glBindVertexArray(0);

   // GLuint positionsBufferLoc = glGetUniformLocation(program.getId(), "positionsBuffer");
   // glUniform3fv(positionsBufferLoc, _positionsBufferSize, &_positionsBuffer);

   
      
// Create timer buffer, this is not of interest to the renderer at the moment
   for(int n = 0; n < PARTICLE_COUNT; ++n) {
      data[3*n+0] = 0.0f; //distribution(generator);
      data[3*n+1] = 0.0f; //distribution(generator);
      data[3*n+2] = 0.0f; //distribution(generator);
   }
// Create Vertex buffer for the timers
   _timerBuffer = OpenCLUtils::createBuffer(_params.context,_params.queue, CL_MEM_READ_WRITE, sizeof(float)*PARTICLE_COUNT, data);
// Create Vertex buffer for the spheres
   _spheresBuffer = OpenCLUtils::createBuffer(_params.context,_params.queue, CL_MEM_READ_WRITE, sizeof(float)*_spheres.size(), _spheres);
   return true;
}

void ParticleSystem::addSphere(const glm::vec3& centre, const float radius){
   _spheres.push_back(centre.x);
   _spheres.push_back(centre.y);
   _spheres.push_back(centre.z);
   _spheres.push_back(radius);
}

void ParticleSystem::addEmitter(const glm::vec3& position, const glm::vec3& dimensions){
   _emitters.push_back(std::make_pair(position, dimensions));
}

bool ParticleSystem::setScenario(const Scenario& s){
   _width = s.getWidth();
   _height = s.getHeight();
   _depth = s.getDepth();
   std::vector<float> textureData = s.getField().getData();

   GLuint glTexture = OpenGLUtils::createTexture3D(_width, _height,_depth, textureData.data());
   int errCode;
   _texture = cl::ImageGL(_params.context,CL_MEM_READ_ONLY,GL_TEXTURE_3D,0,glTexture,&errCode);
   if (errCode!=CL_SUCCESS) {
      std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
      return false;
    }
    return true;
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
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++)
       objs.push_back(_positionsGLBuffer[i]);
   
// Aquire GL Object ( ͡° ͜ʖ ͡°)
   cl_int res = _params.queue.enqueueAcquireGLObjects(&objs,NULL,&ev);
   ev.wait();

   if (res!=CL_SUCCESS) {
      std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
      return;
   }

   _params.kernels.at("particles").setArg(0,_vertexBuffer);
   _params.kernels.at("particles").setArg(1,_spheresBuffer);
   _params.kernels.at("particles").setArg(2,_spheres.size()/4);
   _params.kernels.at("particles").setArg(3,_texture);
   _params.kernels.at("particles").setArg(4,_width);
   _params.kernels.at("particles").setArg(5,_height);
   _params.kernels.at("particles").setArg(6,_depth);
   _params.kernels.at("particles").setArg(7,timeDelta);
   _params.kernels.at("particles").setArg(8,_positionsBuffer);
   _params.kernels.at("particles").setArg(9,PARTICLE_COUNT);
   _params.kernels.at("particles").setArg(10,_positionsBufferHead);

   _positionsBufferHead++;
   _positionsBufferHead %= _positionsBufferSize;
   
   _params.kernels.at("timers").setArg(0,_timerBuffer);
   _params.kernels.at("timers").setArg(1,_vertexBuffer);
   _params.kernels.at("timers").setArg(2,timeDelta);
   _params.kernels.at("timers").setArg(3,20.0f);
   _params.kernels.at("timers").setArg(4,_positionsBuffer);
   _params.kernels.at("timers").setArg(5,PARTICLE_COUNT);
   _params.kernels.at("timers").setArg(6,_positionsBufferSize);

   // Equeue kernel
   _params.queue.enqueueNDRangeKernel(_params.kernels.at("particles"),cl::NullRange,cl::NDRange(getParticleCount(time)),cl::NDRange(1));
   _params.queue.enqueueNDRangeKernel(_params.kernels.at("timers"),cl::NullRange,cl::NDRange(getParticleCount(time)),cl::NDRange(1));

// Copy from OpenCL to OpenGL 
   _params.queue.enqueueCopyBuffer(_vertexBuffer, _tmp, 0, 0, 3*PARTICLE_COUNT*sizeof(float), NULL, NULL);

   size_t stride = _positionsBufferSize/MAX_POSITIONS_BUFFERS;
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++){
       size_t bufferIndex = (_positionsBufferHead + stride*(i+1)) % _positionsBufferSize;
       _params.queue.enqueueCopyBuffer(_positionsBuffer, _positionsGLBuffer[i],
				       bufferIndex*3*PARTICLE_COUNT*sizeof(float), 0,
				       3*PARTICLE_COUNT*sizeof(float), NULL, NULL);
   }
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
