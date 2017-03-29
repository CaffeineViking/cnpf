#include "ParticleSystem.hpp"

#include <iostream>
#include <CL/cl.hpp>

#ifdef WINDOWS
#include <windows.h>
#include <Wingdi.h>
#endif

std::string ParticleSystem::readKernelFile(const std::string& filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if(!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while(!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

ParticleSystem::~ParticleSystem(){

}

// Setup devices and kernel for OpenCL.
// ( ͡° ͜ʖ ͡°)
//
bool ParticleSystem::init(const std::string& path){

  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  if (platforms.empty()){
   std::cerr << "Could not find platforms" << std::endl;
   return false;
} 

for (auto platform : platforms) {

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty()){
       std::cerr << "Could not find valid GPU Device" << std::endl;
       return false;
   }

   _gpuDevice = *(devices.begin());

   std::string deviceInfo;
   _gpuDevice.getInfo(CL_DEVICE_EXTENSIONS, &deviceInfo);
   if(deviceInfo.find("cl_khr_gl_sharing") != std::string::npos){
       _gpuDevice.getInfo(CL_DEVICE_NAME, &deviceInfo);
       std::cout << "GL sharing is supported for device " << deviceInfo << std::endl;
   }
   else{
       _gpuDevice.getInfo(CL_DEVICE_NAME, &deviceInfo);
       std::cerr << "GL sharing is NOT supported for device " << deviceInfo << std::endl;
       return false;
   }
   std::cout<< "Using device: "<<_gpuDevice.getInfo<CL_DEVICE_NAME>()<< std::endl;

   // Setup OpenGL-CL context
   #ifdef WINDOWS

   cl_context_properties properties[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
      CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
      CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
      0
  };

  int status;
  cl_context context = clCreateContext(properties, 1,&_gpuDevice(), NULL, NULL, &status);

    glGenBuffer(1, &hPobj);
  glBindBuffer(GL_ARRAY_BUFFER, hPobj);
  glBufferData(GL_ARRAY_BUFFER, 4 * NUM_PARTICLES * sizeof(float), NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  dPobj = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, hPobj, &status);

  #endif


         // cl_device_id deviceIds[32]; size_t size;
         // clGetGLContextInforHKR(properties, CL_DEVICE_FOR_GL_CONTEXT_KHR, 32 * sizeof(cl_device_id), deviceIds &size);

         // int count = size / sizeof(cl_device_id);
         // cl_context context = clCreateContext(properties, deviceIds, count, NULL, 0, 0);

  _clContext = cl::Context({_gpuDevice});
  std::string kernel_code= readKernelFile(path);
  _clSources.push_back({kernel_code.c_str(),kernel_code.length()});

  _clProgram = cl::Program(_clContext,_clSources);

  if(_clProgram.build({_gpuDevice}) != CL_SUCCESS){
   std::cout<<" Error building: "<<_clProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_gpuDevice) << std::endl;
   return false;
}
return true;

}
return false;
}

void ParticleSystem::compute(){
    cl::Buffer buffer_A(_clContext,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_B(_clContext,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_C(_clContext,CL_MEM_READ_WRITE,sizeof(int)*10);
    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};
    cl::CommandQueue queue(_clContext,_gpuDevice);

    queue.enqueueWriteBuffer(buffer_A,CL_TRUE,0,sizeof(int)*10,A);
    queue.enqueueWriteBuffer(buffer_B,CL_TRUE,0,sizeof(int)*10,B);
    cl::Kernel simple_add(_clProgram, "simple_add");
    simple_add.setArg(0, buffer_A);
    simple_add.setArg(1, buffer_B);
    simple_add.setArg(2, buffer_C);
    queue.enqueueNDRangeKernel(simple_add, cl::NullRange,cl::NDRange(10), cl::NullRange);
    int C[10];
        //read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C,CL_TRUE,0,sizeof(int)*10,C);

    std::cout<<" result: \n";
    for(int i=0;i<10;i++){
        std::cout<<C[i]<<" ";
    }
    std::cout << std::endl;
}

ParticleSystem::ParticleSystem(){


}
