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

TEST_CASE("Check GLFW init") {
    REQUIRE( checkInitGLFW() == true);
}

TEST_CASE("Check GLEW init") {
    REQUIRE( checkInitGLEW() == true);
}
