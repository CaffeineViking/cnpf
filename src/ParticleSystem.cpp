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

// =====================================
// Setup devices and kernel for OpenCL  ( ͡° ͜ʖ ͡°).
// =====================================
bool ParticleSystem::init(const std::string& path){

return false;
}

void ParticleSystem::compute(){
    cl::Buffer buffer_A(_clContext,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_B(_clContext,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_C(_clContext,CL_MEM_READ_WRITE,sizeof(int)*10);
    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
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
