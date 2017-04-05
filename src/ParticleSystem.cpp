#include "ParticleSystem.hpp"

#include <iostream>
#include <CL/cl.hpp>
#include <GL/glew.h>
#include <algorithm>
#include "VectorField3D.hpp"

#ifdef WINDOWS
#include <windows.h>
#endif

ParticleSystem::~ParticleSystem(){}

ParticleSystem::ParticleSystem(const int particles, const float time): PARTICLE_COUNT{particles}, maxTime{time}{
}

// =====================================
// Setup devices and kernel for OpenCL  ( ͡° ͜ʖ ͡°).
// =====================================
bool ParticleSystem::init(const std::string& path, const std::string& kernel, const std::string& device, const ShaderProgram& program){
// Init random kernel with my NVIDIA card. You need to change
   std::cout << "Init OpenCL with device " << device <<  std::endl; 
   _params = OpenCLUtils::initCL(path, kernel, device);

// Load texture and place in GPU
      _width = 32;
      _height = 32;
      _depth = 32;

   std::vector<float> textureData(3*_width*_height*_depth);

   VectorField3D field(glm::vec3(0,0,0), glm::vec3(_width,_height,_depth));
   std::vector<glm::vec3> curled = field.curl().normalize().getField();
   std::cout << curled.size() << std::endl;
   for(int i = 0; i < _width*_height*_depth; i++){
      auto p = curled.at(i) * 0.5f;
      textureData.at((i*3)+0) = p.x + 0.5f;
      textureData.at((i*3)+1) = p.y + 0.5f;
      textureData.at((i*3)+2) = p.z + 0.5f;
   }

   GLuint glTexture = OpenGLUtils::createTexture3D(_width, _height,_depth, textureData.data());
   int errCode;
   _texture = cl::ImageGL(_params.context,CL_MEM_READ_ONLY,GL_TEXTURE_3D,0,glTexture,&errCode);
   if (errCode!=CL_SUCCESS) {
      std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
      return 250;
    }


   std::default_random_engine generator{};
   std::uniform_int_distribution<int> distribution_int(0,_emitters.size()-1);
   std::normal_distribution<float> distribution_float(0.0f,1.0f);
   std::vector<float> data(3*PARTICLE_COUNT);
   for(int n = 0; n < PARTICLE_COUNT; ++n) {
   	auto emitter = _emitters.at(distribution_int(generator));
   	glm::vec3 min = emitter.first - (emitter.second/2.0f);
   	glm::vec3 max = emitter.first + (emitter.second/2.0f);
      float x = min.x + ((float)abs(max.x - min.x))*distribution_float(generator);
      float y = min.y + ((float)abs(max.y - min.y))*distribution_float(generator);
      float z = min.z + ((float)abs(max.z - min.z))*distribution_float(generator);
      data[3*n+0] = x;
      data[3*n+1] = y;
      data[3*n+2] = z;
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

// Create velocity buffer, this is not of interest to the renderer at the moment
   for(int n = 0; n < PARTICLE_COUNT; ++n) {
      data[3*n+0] = 0.0f; //(distribution_float(generator) - 0.5f);
      data[3*n+1] = 0.0f; // (distribution_float(generator) - 0.5f);
      data[3*n+2] = 0.0f;//(distribution_float(generator) - 0.5f);
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

void ParticleSystem::compute(const float time){
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
   _params.kernel.setArg(6,time);

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

