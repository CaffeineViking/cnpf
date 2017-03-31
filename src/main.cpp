// third-party libraries
#ifdef WINDOWS
//#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <CL/cl.hpp>
#include <iostream>

#include <iostream>

#include "Shape.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Locator.hpp"
#include "Texture.hpp"
#include "ParticleSystem.hpp"



const GLuint WIDTH = 800, HEIGHT = 800;

inline unsigned divup(unsigned a, unsigned b)
{
    return (a+b-1)/b;
}

int main()
{

    //====================================
    //  Init for GLFW
    //====================================
    const int MAJOR_VERSION = 2;
    const int MINOR_VERSION = 1;
    std::cout << "Starting GLFW context, OpenGL " << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
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
    glDepthFunc(GL_LESS); 

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  

    //GLuint VertexArrayID;
    //glGenVertexArrays(1,&VertexArrayID);
    //glBindVertexArray(VertexArrayID);

    glViewport(0, 0, width, height);

    //====================================
    //  Init for Shaders and Scenes
    //====================================
    
    // Use default vertex and fragment shader. Fragment makes suff orange and
    // vertrex draw vertecies with camera taken into account.
    Shader vertexShader = Shader("share/shaders/default.vert",GL_VERTEX_SHADER);
    Shader fragmentShader = Shader("share/shaders/default.frag",GL_FRAGMENT_SHADER);
    fragmentShader.compile();
    vertexShader.compile();

    ShaderProgram program{};

    program.attach(fragmentShader);
    program.attach(vertexShader);

    // Create camera to change to MV projection matrix for the vertex shader
    Camera _camera = Camera(45,800,800);
     _camera.translate(glm::vec3(0.0f,12.5f,6.0f));



    // The "Generic" vertex array object which is used to render everyting
    GLuint vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

     ParticleSystem system = ParticleSystem(100000);
     system.init("share/kernels/simple_particle.cl", "simple_particle", "NVIDIA", program);
    // For FPS counter
     double lastTime = glfwGetTime();
     int nbFrames = 0;
     int counter = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {   
         double currentTime = glfwGetTime();
         nbFrames++;
         if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1 sec ago
             // printf and reset timer
            std::cout << "FPS: " << double(nbFrames) << std::endl;
             nbFrames = 0;
             lastTime += 1.0;
         }
         //std::cout << currentTime << std::endl;
        // Poll input
        glfwPollEvents();
     
        system.compute(currentTime);
        // Render vertecies
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         _camera.translate(glm::vec3(0.0f,0.0f,0.0f));
        // _camera.rotate(0.3f);
        _camera.update(program.getId());
       
        program.begin();

        glPointSize(1);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS,0,system.getParticleCount());
        glBindVertexArray(0);
        // Swap the render buffer to display
        glfwSwapBuffers(window);
    }

    //glDeleteVertexArrays(1, &VertexArrayID);
    glfwTerminate();
    return 0;
}

// Kyboard callback for GLFW
