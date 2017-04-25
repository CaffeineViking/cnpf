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
#include "VectorField2D.hpp"
#include "VectorField3D.hpp"
#include <glm/ext.hpp>

const GLuint WIDTH = 1000, HEIGHT = 1000;

int main(int, char**)
{

    //====================================
    //  Init for GLFW
    //====================================
    const int MAJOR_VERSION = 3;
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
    glfwSwapInterval(0);
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
  glViewport(0, 0, width, height);

  //====================================
  //  Init for Shaders and Scenes
  //====================================

  Shader vertexShader = Shader("share/shaders/default.vert",GL_VERTEX_SHADER);
  Shader fragmentShader = Shader("share/shaders/default.frag",GL_FRAGMENT_SHADER);
  fragmentShader.compile();
  vertexShader.compile();

  ShaderProgram program{};

  program.attach(fragmentShader);
  program.attach(vertexShader);

    // Create camera to change to MV projection matrix for the vertex shader
    Camera _camera = Camera(60,WIDTH,HEIGHT);
     _camera.translate(glm::vec3(0.0f,0.5f,2.0f));



    // The "Generic" vertex array object which is used to render everyting
    GLuint vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

      BackwakeScenario backwakeScenario(32,32,32);
      backwakeScenario.generate();
     ParticleSystem system = ParticleSystem(100000, 20.0f);
     system.addEmitter(glm::vec3(0.0f,0.0f,0.0f), glm::vec3( 32.0f,2.0f,32.0f));

     system.init("share/kernels/particles.cl", "particles", "NVIDIA", program);
      system.setScenario(backwakeScenario);

    // For FPS counter
     float currentTime = glfwGetTime();
     float lastFrame = 0.0f;
     float deltaTime = 0.0f;
     float accumulatedTime = 0.0f;

     // glEnable(GL_BLEND);
     //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
       glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Main loop
    while (!glfwWindowShouldClose(window))
    {   
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        accumulatedTime += deltaTime;
        // Poll input
        glfwPollEvents();
        
        if(Locator::input()->isKeyPressed(GLFW_KEY_UP))
          system.compute(accumulatedTime, deltaTime);
        if(Locator::input()->isKeyPressed(GLFW_KEY_DOWN))
          system.compute(accumulatedTime, -deltaTime);
        if(Locator::input()->isKeyPressed(GLFW_KEY_A))
          _camera.rotate(-30.0f * deltaTime);
        if(Locator::input()->isKeyPressed(GLFW_KEY_D))
          _camera.rotate(30.0f  * deltaTime);
        if(Locator::input()->isKeyPressed(GLFW_KEY_W))
          _camera.translate(glm::vec3(0.0f,20.0f*deltaTime,0.0f));
        if(Locator::input()->isKeyPressed(GLFW_KEY_S))
          _camera.translate(glm::vec3(0.0f,-20.0f*deltaTime,0.0f));
        if(Locator::input()->isKeyPressed(GLFW_KEY_C)){

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
           glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Render vertecies
       // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
       //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // _camera.translate(glm::vec3(0.0f,0.1f,0.0f));
         //_camera.rotate(30.0f * deltaTime);
        _camera.update(program.getId());
       
        program.begin();
        glPointSize(1);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS,0,system.getParticleCount(currentTime));
        glBindVertexArray(0);
        // Swap the render buffer to display
        glfwSwapBuffers(window);
    }

    //glDeleteVertexArrays(1, &VertexArrayID);
    glfwTerminate();
    return 0;
}

// Kyboard callback for GLFW
