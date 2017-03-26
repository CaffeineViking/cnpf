#define   CATCH_CONFIG_MAIN
#include "foreign/catch.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSERT( expression ) std::cout<<( (expression) ? "OK" : "NOT OK" )<<std::endl;


bool checkInitGLFW(){

    const int MAJOR_VERSION = 4;
    const int MINOR_VERSION = 3;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
    return true;
}

TEST_CASE("Check GLFW init") {
	REQUIRE( checkInitGLFW() == true);
}

TEST_CASE("Check GLEW init") {
	REQUIRE( checkInitGLEW() == true);
}