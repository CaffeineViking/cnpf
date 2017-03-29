#include "foreign/catch.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <CL/cl.hpp>

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
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) return false;

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

        if (devices.empty()) return false;
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
