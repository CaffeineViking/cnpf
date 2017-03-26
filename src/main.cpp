

// third-party libraries
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "Shape.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Locator.hpp"
#include "Camera.hpp"
const GLuint WIDTH = 800, HEIGHT = 800;

int main()
{

    //====================================
    //  Init for GLFW
    //====================================
    const int MAJOR_VERSION = 4;
    const int MINOR_VERSION = 3;
    std::cout << "Starting GLFW context, OpenGL " << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Default", nullptr, nullptr);    
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set input locators
    GLFWInputLocator* input = new GLFWInputLocator();
    glfwSetCursorPosCallback(window, GLFWInputLocator::cursor_callback);
    glfwSetMouseButtonCallback(window, GLFWInputLocator::mouse_callback);
    glfwSetKeyCallback(window, GLFWInputLocator::keyboard_callback);

    Locator::setInput(input);
    //====================================
    //  Init for GLEW
    //====================================
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }    

    // Setup Z-buffer and Viewport
    glEnable(GL_DEPTH_TEST);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);

    //====================================
    //  Init for Shaders and Scenes
    //====================================

    // Shapes are placed at origin
    Cube cube = Cube{1.0f,1.0f,1.0f};
    
    // Use default vertex and fragment shader. Fragment makes suff orange and
    // vertrex draw vertecies with camera taken into account.
    Shader vertexShader = Shader("assets/shaders/default.vert",GL_VERTEX_SHADER);
    Shader fragmentShader = Shader("assets/shaders/default.frag",GL_FRAGMENT_SHADER);
    fragmentShader.compile();
    vertexShader.compile();

    ShaderProgram program = ShaderProgram{};

    program.attach(fragmentShader);
    program.attach(vertexShader);

    Camera _camera = Camera(45,800,800);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll input
        glfwPollEvents();
        _camera.rotate(0.1f);
        _camera.translate(glm::vec3(0.0f,0.0f,0.0f));
        _camera.update(program.getId());
        // Start of per-frame GL render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);

        program.begin();
        cube.render();
      
        // Swap the render buffer to display
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Kyboard callback for GLFW
