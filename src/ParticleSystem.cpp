#include "ParticleSystem.hpp"

#include <iostream>
#include <CL/cl.hpp>
#include <GL/glew.h>

#ifdef WINDOWS
#include <windows.h>
#include <Wingdi.h>
#endif

ParticleSystem::~ParticleSystem(){

}

ParticleSystem::ParticleSystem(const int particles): PARTICLE_COUNT{particles}{

}

// =====================================
// Setup devices and kernel for OpenCL  ( ͡° ͜ʖ ͡°).
// =====================================
bool ParticleSystem::init(const std::string& path, const std::string& kernel, const std::string& device, const ShaderProgram& program){
// Init random kernel with my NVIDIA card. You need to change
   std::cout << "Init OpenCL with device " << device <<  std::endl; 
   _params = OpenCLUtils::initCL(path, kernel, device);

   int width = sqrt(PARTICLE_COUNT);
   int height = width ;
   std::cout << width << " " << height << std::endl;
   std::vector<float> data(3*PARTICLE_COUNT);
   for(int n = 0; n < PARTICLE_COUNT; ++n) {
      int x = n % width;
      int y = n / width;
      data[3*n+0] = (x - width/2) / 100.0f;
      data[3*n+1] = 0.0f;
      data[3*n+2] = (y - height/2) / 100.0f;
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
      int x = n % width;
      int y = n / width;
      data[3*n+0] = 0.0f;
      data[3*n+1] = 0.0f;
      data[3*n+2] = 0.0f;
   }
// Create Vertex buffer for the positions
   _velocityBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*PARTICLE_COUNT);
// Write position data to buffer
   _params.queue.enqueueWriteBuffer(_velocityBuffer, CL_TRUE, 0, sizeof(float)*3*PARTICLE_COUNT, data.data());
   return true;
}

void ParticleSystem::compute(const float time){
// CL event used to wait for kernel osv...
   cl::Event ev;
     // set kernel arguments
     //======================================================
    // Vertex array object buffer with coords interleaved
    //======================================================
   _params.kernel.setArg(0,_vertexBuffer);
   _params.kernel.setArg(1,_velocityBuffer);
   _params.kernel.setArg(2,time);
     // Equeue kernel
   _params.queue.enqueueNDRangeKernel(_params.kernel,cl::NullRange,cl::NDRange(PARTICLE_COUNT),cl::NDRange(1));
     // Wait for kernel
   glFinish();

   std::vector<cl::Memory> objs;
   objs.clear();
   objs.push_back(_tmp);

     // Aqquire GL Object ( ͡° ͜ʖ ͡°)
   cl_int res = _params.queue.enqueueAcquireGLObjects(&objs,NULL,&ev);
   ev.wait();

   if (res!=CL_SUCCESS) {
      std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
      return;
   }

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

int ParticleSystem::getParticleCount() const {
   return PARTICLE_COUNT;
}

