#include "ParticleSystem.hpp"

#include <random>
#include <iostream>
#include <GL/glew.h>
#include <CL/cl.hpp>
#include <algorithm>
#include "VectorField3D.hpp"
#include <limits>
#include "Noise.hpp"
#include <algorithm>
#ifdef WINDOWS
#include <windows.h>
#endif

#define OUTPUT_WIDTH 1024
#define OUTPUT_HEIGHT 1024

ParticleSystem::~ParticleSystem(){
    // Don't forget to clean this up!
    glDeleteBuffers(1, &_vertexBufferId);
}

ParticleSystem::ParticleSystem(const int maxParticles):
 _currentParticles{0}, 
 _maxParticleCount{maxParticles}, 
 _particlePerFrame{10},
 _respawnTime{20.0f},
 _fieldMagnitude{1.0f},
 _noiseRatio{0.0f},
  _noiseMagnitude{1.0f},
 _lengthScale{20.0f},
 _fieldDirection{0.0f,-1.0f,0.0f}
{
}


// =====================================
// Setup devices and kernel for OpenCL  ( ͡° ͜ʖ ͡°).
// =====================================
bool ParticleSystem::init(std::vector<std::string> paths, std::vector<std::string> kernels, const std::string& device, const ShaderProgram& program){
// Init random kernel with my NVIDIA card. You need to change
   std::cout << "Init OpenCL with device " << device <<  std::endl; 
   _params = OpenCLUtils::initCL(paths, kernels, device);

   std::vector<float> spawnerData;
   for(int i = 0; i < _emitters.size(); i++){
      spawnerData.push_back(_emitters.at(i).first.x);
      spawnerData.push_back(_emitters.at(i).first.y);
      spawnerData.push_back(_emitters.at(i).first.z);
      spawnerData.push_back(_emitters.at(i).second.x);
      spawnerData.push_back(_emitters.at(i).second.y);
      spawnerData.push_back(_emitters.at(i).second.z);
   }

   _spawnerBuffer = OpenCLUtils::createBuffer(_params.context,_params.queue, CL_MEM_READ_WRITE, sizeof(float)*_emitters.size()*6, spawnerData);

   std::vector<float> data(3*_maxParticleCount);

   _positionsBufferSize = 64;
   _positionsBufferHead = 0;

   std::vector<float> positionsBufferData(4*3*_maxParticleCount);
   for(int i = 0; i < 4; i++)
       std::copy(data.begin(), data.end(), positionsBufferData.begin() + i*data.size());
   

   // Same but for OpenGL
   _vertexBufferId = OpenGLUtils::createBuffer(3*_maxParticleCount, data.data(), GL_DYNAMIC_DRAW);
// _tmp buffer to do some copying on
   _tmp = cl::BufferGL(_params.context, CL_MEM_READ_WRITE, _vertexBufferId, NULL);
   
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++){
       _positionsBufferId[i] = OpenGLUtils::createBuffer(3*_maxParticleCount, data.data(), GL_DYNAMIC_DRAW);
       _positionsGLBuffer[i] = cl::BufferGL(_params.context, CL_MEM_READ_WRITE, _positionsBufferId[i], NULL);
   }

// Create Vertex buffer for the positions
   _vertexBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*_maxParticleCount);
   _positionsBuffer = cl::Buffer(_params.context, CL_MEM_READ_WRITE, sizeof(float)*3*_maxParticleCount*_positionsBufferSize);

   for(int i = 0; i < _positionsBufferSize; i++){
       _params.queue.enqueueWriteBuffer(_positionsBuffer, CL_TRUE, i*sizeof(float)*3*_maxParticleCount, sizeof(float)*3*_maxParticleCount, data.data());
   }
   // Write position data to buffer
   _params.queue.enqueueWriteBuffer(_vertexBuffer, CL_TRUE, 0, sizeof(float)*3*_maxParticleCount, data.data());
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
    std::fill(data.begin(), data.end(),999);
// Create Vertex buffer for the timers
   _timerBuffer = OpenCLUtils::createBuffer(_params.context,_params.queue, CL_MEM_READ_WRITE, sizeof(float)*_maxParticleCount, data);
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

void ParticleSystem::compute(const float time, const float timeDelta){

_currentParticles += _particlePerFrame;
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
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++)
       objs.push_back(_positionsGLBuffer[i]);
   
// Aquire GL Object ( ͡° ͜ʖ ͡°)
   cl_int res = _params.queue.enqueueAcquireGLObjects(&objs,NULL,&ev);
   ev.wait();

   if (res!=CL_SUCCESS) {
      std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
      return;
   }

   Params kernelParameters{};
   kernelParameters.width = _width;
   kernelParameters.height = _height;
   kernelParameters.depth = _depth;
   kernelParameters.fieldMagnitude = _fieldMagnitude;
   kernelParameters.noiseRatio = _noiseRatio;
   kernelParameters.lengthScale = _lengthScale;
   kernelParameters.noiseMagnitude = _noiseMagnitude;
   kernelParameters.boundraryWidth = 1.0f;
   kernelParameters.fieldDirection = _fieldDirection;
   _params.kernels.at("particles").setArg(0,_vertexBuffer);
   _params.kernels.at("particles").setArg(1,_spheresBuffer);
   _params.kernels.at("particles").setArg(2,_spheres.size()/4);
   _params.kernels.at("particles").setArg(3,kernelParameters);
   _params.kernels.at("particles").setArg(4,timeDelta);
   _params.kernels.at("particles").setArg(5,_positionsBuffer);
   _params.kernels.at("particles").setArg(6,_maxParticleCount);
   _params.kernels.at("particles").setArg(7,_positionsBufferHead);

   _positionsBufferHead++;
   _positionsBufferHead %= _positionsBufferSize;
   
   _params.kernels.at("timers").setArg(0,_timerBuffer);
   _params.kernels.at("timers").setArg(1,_vertexBuffer);
   _params.kernels.at("timers").setArg(2,timeDelta);
   _params.kernels.at("timers").setArg(3,_respawnTime);
   _params.kernels.at("timers").setArg(4,_positionsBuffer);
   _params.kernels.at("timers").setArg(5,_maxParticleCount);
   _params.kernels.at("timers").setArg(6,_positionsBufferSize);
   _params.kernels.at("timers").setArg(7,_spawnerBuffer);
   _params.kernels.at("timers").setArg(8,_emitters.size());


   // Enqueue kernel for snapshoting field
  
   // Equeue kernel
   _params.queue.enqueueNDRangeKernel(_params.kernels.at("particles"),cl::NullRange,cl::NDRange(getParticleCount(time)),cl::NDRange(1));
   _params.queue.enqueueNDRangeKernel(_params.kernels.at("timers"),cl::NullRange,cl::NDRange(getParticleCount(time)),cl::NDRange(1));

   // Copy from OpenCL to OpenGL 
   _params.queue.enqueueCopyBuffer(_vertexBuffer, _tmp, 0, 0, 3*_maxParticleCount*sizeof(float), NULL, NULL);
   size_t stride = _positionsBufferSize/MAX_POSITIONS_BUFFERS;
   for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++){
       size_t bufferIndex = (_positionsBufferHead + stride*(i+1)) % _positionsBufferSize;
       _params.queue.enqueueCopyBuffer(_positionsBuffer, _positionsGLBuffer[i],
				       bufferIndex*3*_maxParticleCount*sizeof(float), 0,
				       3*_maxParticleCount*sizeof(float), NULL, NULL);
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

  return std::min(_currentParticles, _maxParticleCount);
}

float* ParticleSystem::referenceRespawnTime() {
   return &_respawnTime;
}

float* ParticleSystem::referenceFieldMagnitude(){
   return &_fieldMagnitude;
}

float* ParticleSystem::referenceNoiseRatio() {
   return &_noiseRatio;
}


int* ParticleSystem::referenceParticlesPerFrame() {
   return &_particlePerFrame;
}

glm::vec3* ParticleSystem::referenceFieldDirection() {
   return &_fieldDirection;

}

float* ParticleSystem::referenceLengthScale() {
  return &_lengthScale;
}

float* ParticleSystem::referenceNoiseMagnitude() {
  return &_noiseMagnitude;
}

// A real bastard of a function
bool ParticleSystem::snapshot(const std::string& filePath, const std::string& kenelName) {

const unsigned w = 2048;
const unsigned h = 2048;
const float scaleFactor = 0.018f;
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
   objs.push_back(_positionsGLBuffer[0]);
// Aquire GL Object ( ͡° ͜ʖ ͡°)
   cl_int res = _params.queue.enqueueAcquireGLObjects(&objs,NULL,&ev);
   ev.wait();

   if (res!=CL_SUCCESS) {
      std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
      return false;
   }

// Create Image to store field output in
   const cl::ImageFormat format(CL_RGBA, CL_FLOAT);
   cl::Image2D outputImage = cl::Image2D(_params.context, CL_MEM_READ_WRITE, format, w,h);

    std::vector<float> data(w * h * 4 * sizeof(float));
    std::cout << data.size() << std::endl;
    for(unsigned i = 0; i < data.size(); i++){
      data[i] = 0.1f;
    }

    cl::size_t<3> origin;
    origin[0] = 0; 
    origin[1] = 0; 
    origin[2] = 0;

    cl::size_t<3> region;
    region[0] = w;
    region[1] = h;
    region[2] = 1;

//    _params.queue.enqueueWriteImage(outputImage, CL_TRUE, origin, region,0,0, data.data());

   Params kernelParameters{};
   kernelParameters.width = w;
   kernelParameters.height = 1;
   kernelParameters.depth = h;
   kernelParameters.fieldMagnitude = _fieldMagnitude;
   kernelParameters.noiseRatio = _noiseRatio;
   kernelParameters.lengthScale = _lengthScale;
   kernelParameters.noiseMagnitude = _noiseMagnitude;
   kernelParameters.boundraryWidth = 1.0f;
   kernelParameters.fieldDirection = _fieldDirection;


   _params.kernels.at(kenelName).setArg(0,outputImage);
   _params.kernels.at(kenelName).setArg(1,_spheresBuffer);
   _params.kernels.at(kenelName).setArg(2,_spheres.size()/4);
   _params.kernels.at(kenelName).setArg(3,kernelParameters);
   _params.kernels.at(kenelName).setArg(4,scaleFactor);

   cl_int err;
   err = _params.queue.enqueueNDRangeKernel(_params.kernels.at(kenelName),cl::NDRange(0,0),cl::NDRange(w,h),cl::NDRange(1,1));
   if(err != 0){
    std::cout << "Something went wrong when running kernel: "<< kenelName << " " << err << std::endl;
    return false;
   }

   res = _params.queue.enqueueReleaseGLObjects(&objs);
   ev.wait();
   if (res!=CL_SUCCESS) {
      std::cout<<"Failed releasing GL object: "<<res<<std::endl;
      return false;
   }

   _params.queue.finish();

    for(unsigned i = 0; i < data.size(); i++){
      data[i] = 0.0f;
    }

    _params.queue.enqueueReadImage(outputImage, CL_TRUE, origin, region,0,0, data.data());
    OpenGLUtils::writePNG(filePath, w, h, data);

   return true;
}
