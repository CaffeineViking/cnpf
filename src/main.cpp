#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <CL/cl.hpp>
#include <iostream>

#include "Shape.hpp"
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Locator.hpp"
#include "Texture.hpp"
#include "ParticleSystem.hpp"
#include "ParticleRenderer.hpp"
#include "VectorField2D.hpp"
#include "VectorField3D.hpp"
#include "MovingCamera.hpp"
#include <glm/ext.hpp>
#include "OpenGLUtils.hpp"

#include "stringPatch.hpp"

const std::string TITLE = "Curl-Noise Particle Field @ ";
const GLuint WIDTH = 1280, HEIGHT = 720;

inline unsigned divup(unsigned a, unsigned b)
{
    return (a+b-1)/b;
}

int main(int argc, char**argv)
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

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE.c_str(), nullptr, nullptr);
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
    glfwSwapInterval(1);
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

    // Setup the Z-buffer
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    // Setup the draw buffer blending function.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //====================================
    //  Init for Shaders and Scenes
    //====================================

    Shader vertexShader = Shader("share/shaders/default.vert",GL_VERTEX_SHADER);
    Shader fragmentShader = Shader("share/shaders/default.frag",GL_FRAGMENT_SHADER);
    Shader geometryShader = Shader("share/shaders/default.geo",GL_GEOMETRY_SHADER);

    fragmentShader.compile();
    vertexShader.compile();
    geometryShader.compile();

    ShaderProgram program{};
    program.attach(vertexShader);
    program.attach(geometryShader);
    program.attach(fragmentShader);

    // Create camera to change to MV projection matrix for the vertex shader
    MovingCamera _camera = MovingCamera(glm::radians(100.0f),WIDTH,HEIGHT);
    _camera.getTransform()->translate(glm::vec3(0.0f,0.5f,-10.0f));

    // The "Generic" vertex array object which is used to render everyting
    GLuint vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    ParticleSystem system = ParticleSystem(200000, 1.0f);
    system.addEmitter(glm::vec3(0.0f,1.0f,0.0f), glm::vec3(4.0f,4.0f,4.0f));

    system.init("share/kernels/particles.cl", "particles", "AMD", program);

    unsigned w, h;
    std::vector<float> diffuseData;
    if(!OpenGLUtils::loadPNG("share/textures/fire.png", w, h, diffuseData)){
        std::cerr << "Failed to load image..." << std::endl;
        return 1;
    }

    Texture diffuse(w, h, diffuseData.data()); diffuse.begin(0);
    GLuint location = glGetUniformLocation(program.getId(), "diffuse");
    glUniform1i(location, diffuse.getId());

    // Keep track of slowdown/speedup.
    float currentTime = glfwGetTime();
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    float accumulatedTime = 0.0f;

    // For counting the framerate.
    unsigned accumulatedFrames = 0;
    float frameAccumulatedTime = 0.0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        accumulatedTime += deltaTime;
        accumulatedFrames += 1;

        // We accumulate the number of frames every second and print out this
        // number. This is better since we don't update the title every frame
        // and produces more stable results. Just resets counter and repeats.
        if (currentTime - frameAccumulatedTime >= 1.0) {
            const std::string fpsString = patch::to_string(accumulatedFrames);
            const std::string windowTitle = TITLE + fpsString + " fps";
            glfwSetWindowTitle(window, windowTitle.c_str());
            frameAccumulatedTime = currentTime;
            accumulatedFrames = 0;
        }

        // Polling loop.
        glfwPollEvents();

        // // Compute the next step in the particle simpulation
        // // where we issue each step in parallel using a GPU.
        // system.compute(accumulatedTime, deltaTime);

        // Clear the relevant OpenGL buffers.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _camera.handleInput(deltaTime);
        _camera.update(program.getId());

        program.begin();
        diffuse.begin(0);
        glPointSize(1);
        glBindVertexArray(vao);
        // Finally, render the vertices or quads to the back buffer!
        glDrawArrays(GL_POINTS, 0, system.getParticleCount(currentTime));
        glBindVertexArray(0);

        // Swap the render buffer to display
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
