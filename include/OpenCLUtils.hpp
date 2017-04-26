#ifndef OPENCLUTILS_HPP
#define OPENCLUTILS_HPP

#include <CL/cl.hpp>
#include <vector>
#include <map>
typedef struct {
	cl::Device device;
    cl::CommandQueue queue;
    cl::Program program;
	std::map<std::string,cl::Kernel> kernels;
	cl::Context context;

} clParameters;

class OpenCLUtils{
private:
	static std::string readFile(const std::string& filePath);

public:
	static cl::Platform getPlatform(const std::string&);
	static bool checkExtensionSupport(const cl::Device&, const std::string&);
	static cl::Program getProgram(const cl::Context&, std::vector<std::string>);
	static clParameters initCL(std::vector<std::string>, std::vector<std::string>, const std::string&);
	static void dumpInfo();
};
#endif