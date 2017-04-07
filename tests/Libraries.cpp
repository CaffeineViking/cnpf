#include "foreign/catch.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <CL/cl.hpp>
#include "OpenCLUtils.hpp"
bool checkInitGLFW(){

    const int MAJOR_VERSION = 2;
    const int MINOR_VERSION = 1;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(200, 200, "Default", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    return true;
}

bool checkInitGLEW(){

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        return false;
    }

    std::cerr << "Context vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cerr << "Context renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cerr << "Context version: " << glGetString(GL_VERSION) << std::endl;
    std::cerr << "Context shaders: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cerr << std::endl;

    return true;
}

bool checkInitOpenCL() {
    //OpenCLUtils::dumpInfo();
    clParameters clParams = OpenCLUtils::initCL("share/kernels/simple_add.cl", "simple_add", "NVIDIA");

    cl::Buffer buffer_A(clParams.context,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_B(clParams.context,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_C(clParams.context,CL_MEM_READ_WRITE,sizeof(int)*10);
    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    clParams.queue.enqueueWriteBuffer(buffer_A,CL_TRUE,0,sizeof(int)*10,A);
    clParams.queue.enqueueWriteBuffer(buffer_B,CL_TRUE,0,sizeof(int)*10,B);
    clParams.kernel.setArg(0, buffer_A);
    clParams.kernel.setArg(1, buffer_B);
    clParams.kernel.setArg(2, buffer_C);
    clParams.queue.enqueueNDRangeKernel(clParams.kernel, cl::NullRange,cl::NDRange(10), cl::NullRange);
    int C[10];
    clParams.queue.enqueueReadBuffer(buffer_C,CL_TRUE,0,sizeof(int)*10,C);

    int expectedResult[] = {0,2,4,6,8,10,12,14,16,18};
    for(int i=0;i<10;i++){
       if(expectedResult[i] != C[i])
        return false;
    }

    return true;
}

TEST_CASE("Check GLFW init") {
    REQUIRE( checkInitGLFW() == true);
}

TEST_CASE("Check GLEW init") {
    REQUIRE( checkInitGLEW() == true);
}

TEST_CASE("Check OpenCL init") {
    REQUIRE(checkInitOpenCL() == true);
}
