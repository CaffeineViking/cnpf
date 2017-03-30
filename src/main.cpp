

// third-party libraries
#ifdef WINDOWS
//#include <windows.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <CL/cl.hpp>
#include <iostream>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

#include "Shape.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Locator.hpp"
#include "Texture.hpp"
#include "ParticleSystem.hpp"

#include "OpenCLUtils.hpp"
#include "OpenGLUtils.hpp"

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

    ShaderProgram program = ShaderProgram{};

    program.attach(fragmentShader);
    program.attach(vertexShader);

    // Create camera to change to MV projection matrix for the vertex shader
    Camera _camera = Camera(45,800,800);
     _camera.translate(glm::vec3(0.0f,0.5f,1.0f));

     // Init random kernel with my NVIDIA card. You need to change
     std::cout << "Init OpenCL with device NVIDIA hardcoded" << std::endl; 
    clParameters clParams = OpenCLUtils::initCL("share/kernels/random.cl", "random", "NVIDIA");

    const int PARTICLE_COUNT = 1000000;
        std::random_device rd;
        std::mt19937 eng(rd());
        std::normal_distribution<> dist(1, 100);
        std::vector<float> data(3*PARTICLE_COUNT);
        for(int n=0; n<PARTICLE_COUNT; ++n) {
            float x = n %  1000 - 500;
            float y = n/ 1000 - 500;
            data[3*n+0] = x / 1000;
            data[3*n+1] = 0.0f;
            data[3*n+2] = y / 1000;
        }
    // Create Vertex buffer for the positions
    cl::Buffer buffer = cl::Buffer(clParams.context, CL_MEM_READ_WRITE, sizeof(float)*3*PARTICLE_COUNT);

    // Same but for OpenGL
    GLuint vbo = OpenGLUtils::createBuffer(3*PARTICLE_COUNT, data.data(), GL_STATIC_DRAW);

    // Tmp buffer to do some copying on
    cl::BufferGL tmp = cl::BufferGL(clParams.context, CL_MEM_READ_WRITE, vbo, NULL);

    // The "Generic" vertex array object which is used to render everyting
    GLuint vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    // Bind buffer to shader
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    GLint position_attribute = glGetAttribLocation(program.getId(), "position");
    glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(position_attribute);
    glBindVertexArray(0);

    // Write position data to buffer
    clParams.queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, sizeof(float)*3*PARTICLE_COUNT, data.data());

    cl::size_t<3> dimensions;
    dimensions[0] = PARTICLE_COUNT;
    dimensions[1] = 1;
    dimensions[2] = 1;

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
        // std::cout << "Hello World " << std::endl;
        // Poll input
        glfwPollEvents();
     
        // CL event used to wait for kernel osv...
        cl::Event ev;
        // Something number of particles something...
        cl::NDRange local(1);
        cl::NDRange global(1 *  divup(dimensions[0],1));

        // set kernel arguments
        //======================================================
        // Vertex array object buffer with coords interleaved
        //======================================================
        clParams.kernel.setArg(0,buffer);
        // Width
        clParams.kernel.setArg(1,800);
        // Height
        clParams.kernel.setArg(2,800);

        //Random seed for GLSL psuedo random generator
        clParams.kernel.setArg(3,counter++);

        // Equeue kernel
        clParams.queue.enqueueNDRangeKernel(clParams.kernel,cl::NullRange,global,local);
        // Wait for kernel
        glFinish();

        std::vector<cl::Memory> objs;
        objs.clear();
        objs.push_back(tmp);

        // Aqquire GL Object ( ͡° ͜ʖ ͡°)
        cl_int res = clParams.queue.enqueueAcquireGLObjects(&objs,NULL,&ev);
        ev.wait();

        if (res!=CL_SUCCESS) {
            std::cout<<"Failed acquiring GL object: "<<res<<std::endl;
            exit(248);
        }

        // Copy from OpenCL to OpenGL 
        clParams.queue.enqueueCopyBuffer(buffer, tmp, 0, 0, 3*PARTICLE_COUNT*sizeof(float), NULL, NULL);
        res = clParams.queue.enqueueReleaseGLObjects(&objs);
        ev.wait();
        if (res!=CL_SUCCESS) {
            std::cout<<"Failed releasing GL object: "<<res<<std::endl;
            exit(247);
        }

        // Wait for copy to be done
        clParams.queue.finish();

        // Render vertecies
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         _camera.translate(glm::vec3(0.0f,0.0f,0.0f));
         _camera.rotate(0.3f);
        _camera.update(program.getId());
       
        program.begin();

        glPointSize(1);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS,0,PARTICLE_COUNT);
        glBindVertexArray(0);
        // Swap the render buffer to display
        glfwSwapBuffers(window);
    }

    //glDeleteVertexArrays(1, &VertexArrayID);
    glfwTerminate();
    return 0;
}

// Kyboard callback for GLFW
