#include "OpenCLUtils.hpp"
#include <iostream>
#include <fstream>

#ifdef WINDOWS
#include <windows.h>
#include <Wingdi.h>
#elif UNIX
#include <GL/glx.h>
#endif

const std::string OpenCLUtils::getErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}


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
	std::string content = "";
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

int read_file(unsigned char **output, size_t *size, const char *name) {
  FILE* fp = fopen(name, "rb");
  if (!fp) {
    return -1;
  }

  fseek(fp, 0, SEEK_END);
  *size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  *output = (unsigned char *)malloc(*size);
  if (!*output) {
    fclose(fp);
    return -1;
  }

  fread(*output, *size, 1, fp);
  fclose(fp);
  return 0;
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
cl::Program OpenCLUtils::getProgram(const cl::Context& context, std::vector<std::string> paths){

	cl::Program::Sources sources;

	std::vector<std::string> stringSources;
	for(unsigned i = 0;i < paths.size(); i++){
		stringSources.push_back(readFile(paths.at(i)));
		sources.push_back(std::make_pair(stringSources.at(i).c_str(), stringSources.at(i).length()));
	}

	//std::cout << sources.at(0).first << std::endl;
	cl::Program program(context, sources);

	return program;

}

cl::Program OpenCLUtils::getProgramFromBinary(const cl::Context& context, std::vector<cl::Device> devices, std::vector<std::string> paths){
	cl::Program::Binaries binaries;
	cl_int err;

	std::vector<std::string> stringBinaries;
	unsigned char* program_file = NULL;
	size_t program_size = 0;
	for(unsigned i = 0; i < paths.size(); i++){
	    read_file(&program_file, &program_size, paths.at(i).c_str());
	    binaries.push_back(std::make_pair(program_file, program_size));
	}

	std::vector<cl_int> status;
	cl::Program program(context, devices, binaries, &status, &err);
	if(err != CL_SUCCESS){
	    std::cerr << "Could not create program from binary: " << getErrorString(err) << std::endl;
	}

	return program;
}


//=============================
// OpenCL Setup and parameters
//=============================
clParameters OpenCLUtils::initCL(std::vector<std::string> paths, std::vector<std::string> kernels, const std::string& platformName, const bool fromBinary){
	// Return value
	clParameters clParams{};
	cl::Platform platform = getPlatform(platformName);

	// Construct properties, platform dependent
	cl_context_properties cps[] = {
	#ifdef WINDOWS
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
		0
	#elif UNIX
		CL_GL_CONTEXT_KHR,   (cl_context_properties)glXGetCurrentContext(),
		CL_GLX_DISPLAY_KHR,  (cl_context_properties)glXGetCurrentDisplay(),
		CL_CONTEXT_PLATFORM, (cl_context_properties)platform(),
		0
	#endif
	};

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

	if(fromBinary){
	    clParams.program = getProgramFromBinary(context, std::vector<cl::Device>{clParams.device}, paths);
	} else {
	    clParams.program = getProgram(context, paths);
	}

	if(clParams.program.build({clParams.device}) != CL_SUCCESS){
	    std::cout<<" Error building: "<< clParams.program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(clParams.device) << std::endl;
	    return clParams;
	}

	std::string tmp;
	for(std::string kernel: kernels){
		tmp = kernel;
		clParams.kernels[kernel] = cl::Kernel{clParams.program, tmp.c_str()};
	}

	return clParams;
}

cl::Buffer OpenCLUtils::createBuffer(const cl::Context& context,const cl::CommandQueue& queue,int flags, const unsigned size, const std::vector<float>& data){
	cl::Buffer buffer = cl::Buffer(context, flags, size);
   queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, size, data.data());
   return buffer;
}

cl::ImageGL OpenCLUtils::createTexture(const cl::Context& context,int flags, GLenum type,  GLuint glTexture){
	int errCode;
	cl::ImageGL texture = cl::ImageGL(context,flags,type,0,glTexture,&errCode);
   if (errCode!=CL_SUCCESS) {
      std::cout<<"Failed to create OpenGL texture refrence: "<<errCode<<std::endl;
    }
    return texture;
}
