#ifndef OPENCLUTILS_HPP
#define OPENCLUTILS_HPP

#include <CL/cl.hpp>

typedef struct {
	cl::Device device;
    cl::CommandQueue queue;
    cl::Program program;
	cl::Kernel kernel;
	cl::Context context;

} clParameters;

class OpenCLUtils{
private:
	static std::string readFile(const std::string& filePath);

public:
	static cl::Platform getPlatform(const std::string&);
	static bool checkExtensionSupport(const cl::Device&, const std::string&);
	static cl::Program getProgram(const cl::Context&, const std::string&);
	static clParameters initCL(const std::string&, const std::string&, const std::string&);
	static void dumpInfo();
};
#endif