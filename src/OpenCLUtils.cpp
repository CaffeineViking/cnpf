#include "OpenCLUtils.hpp"
#include <iostream>
#include <fstream>
#include <CL/cl.hpp>

#ifdef _WIN32
#include <windows.h>
#include <Wingdi.h>
#endif

void OpenCLUtils::dumpInfo(){
  std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) return;

    std::cerr << "Number of platforms found: " << platforms.size() << std::endl;
    for (auto platform : platforms) {
        std::string platformInfo;
        platform.getInfo(CL_PLATFORM_PROFILE, &platformInfo);
        std::cerr << "Platform profile: " << platformInfo << std::endl;
        platform.getInfo(CL_PLATFORM_VENDOR, &platformInfo);
        std::cerr << "Platform vendor: " << platformInfo << std::endl;
        platform.getInfo(CL_PLATFORM_NAME, &platformInfo);
        std::cerr << "Platform name: " << platformInfo << std::endl;
        platform.getInfo(CL_PLATFORM_VERSION, &platformInfo);
        std::cerr << "Platform version: " << platformInfo << std::endl;
        platform.getInfo(CL_PLATFORM_EXTENSIONS, &platformInfo);
        std::cerr << "Platform extensions: " << platformInfo << std::endl;
        std::cout << std::endl;

        std::vector<cl::Device> devices;
        platform.getDevices(CL_DEVICE_TYPE_CPU, &devices);
        std::cout << "Devices of type CPU:" << std::endl;
        for (auto device : devices) {
            std::string deviceInfo;
            device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &deviceInfo);
            std::cerr << "Device max work items: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_NAME, &deviceInfo);
            std::cerr << "Device name: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_VERSION, &deviceInfo);
            std::cerr << "Device version: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_PROFILE, &deviceInfo);
            std::cerr << "Device profile: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_VERSION, &deviceInfo);
            std::cerr << "Device version: " << deviceInfo << std::endl;
            device.getInfo(CL_DRIVER_VERSION, &deviceInfo);
            std::cerr << "Driver version: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_OPENCL_C_VERSION, &deviceInfo);
            std::cerr << "Device OpenCL version: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_EXTENSIONS, &deviceInfo);
            std::cerr << "Device extensions: " << deviceInfo << std::endl;
        } std::cerr << std::endl;

        platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
        std::cout << "Devices of type GPU:" << std::endl;
        for (auto device : devices) {
            std::string deviceInfo;
            device.getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &deviceInfo);
            std::cerr << "Device max work items: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_NAME, &deviceInfo);
            std::cerr << "Device name: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_VERSION, &deviceInfo);
            std::cerr << "Device version: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_PROFILE, &deviceInfo);
            std::cerr << "Device profile: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_VERSION, &deviceInfo);
            std::cerr << "Device version: " << deviceInfo << std::endl;
            device.getInfo(CL_DRIVER_VERSION, &deviceInfo);
            std::cerr << "Driver version: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_OPENCL_C_VERSION, &deviceInfo);
            std::cerr << "Device OpenCL version: " << deviceInfo << std::endl;
            device.getInfo(CL_DEVICE_EXTENSIONS, &deviceInfo);
            std::cerr << "Device extensions: " << deviceInfo << std::endl;
        } std::cerr << std::endl;

        if (devices.empty()) return;
    }

    return;
}
//=========================
// Read OpenCL Kernel file
//=========================
std::string OpenCLUtils::readFile(const std::string& filePath){
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

//===========================
// Get platform given a name
//===========================
cl::Platform OpenCLUtils::getPlatform(const std::string& name){
	std::vector<cl::Platform> platforms{};
	cl::Platform::get(&platforms);
	for(cl::Platform platform: platforms){
		std::string platformVendor = platform.getInfo<CL_PLATFORM_NAME>();
		if(platformVendor.find(name) != std::string::npos){
			return platform;
		}
	}
	cl::Platform platform;
	return platform;
}

//===================================================
// Check if device support specific OpenCL Extension
//===================================================
bool OpenCLUtils::checkExtensionSupport(const cl::Device& device, const std::string& extensionName){

	std::string extension = device.getInfo<CL_DEVICE_EXTENSIONS>();
	if(extension.find(extensionName) != std::string::npos){
		return true;
	}
	return false;
}

////==============================================
// Get OpenCL Program given file path and context
//================================================
cl::Program OpenCLUtils::getProgram(const cl::Context& context, const std::string& filePath){

	std::string sourceCode = readFile(filePath);
	cl::Program::Sources sources{1, std::make_pair(sourceCode.c_str(),sourceCode.length()+1)};
	cl::Program program{context, sources};
	return program;

}

//=============================
// OpenCL Setup and parameters
//=============================
clParameters OpenCLUtils::initCL(const std::string& filePath, const std::string& kernelName, const std::string& platformName){
	// Return value
	clParameters clParams{};
	cl::Platform platform = getPlatform(platformName);

	// Construct properties, platform dependent
	#ifdef _WIN32
	cl_context_properties cps[] = {
      CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
      CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
      CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
		0
	};
	#endif

	// Make sure we find platform with device that supports GL_SHARING
	std::vector<cl::Device> devices{};
	platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	for(cl::Device device: devices){
		if(checkExtensionSupport(device, "cl_khr_gl_sharing")){
			clParams.device = device;
			break;
		}
	}

	// Build openCL kernel and program
	cl::Context context{clParams.device, cps};
	clParams.context = context;
	clParams.queue = cl::CommandQueue{context,clParams.device};
	clParams.program = getProgram(context, filePath);
	if(clParams.program.build({clParams.device}) != CL_SUCCESS){
   		std::cout<<" Error building: "<< clParams.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(clParams.device) << std::endl;
   		return clParams;
	}

	clParams.kernel = cl::Kernel{clParams.program, kernelName.c_str()};

	return clParams;
}