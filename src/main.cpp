#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <CL/cl.hpp>
#include <iostream>

#include "Locator.hpp"
#include "Scenario.hpp"
#include "ParticleSystem.hpp"
#include "ParticleRenderer.hpp"
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

int main(int, char**)
{

    //====================================
    //  Init for GLFW
    //====================================
    const int MAJOR_VERSION = 3;
    const int MINOR_VERSION = 1;
    std::cout << "Starting GLFW context, OpenGL "
        << MAJOR_VERSION << "."
        << MINOR_VERSION << std::endl;

    if (!glfwInit()) return -1;
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
    glfwSwapInterval(1); // Vsync.

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

    // Setup the Z-buffer
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    // Setup our buffer blending method.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    //====================================
    //  Init Particle System and Renderer
    //====================================

    // Create camera to create a MVP matrix for the shader program.
    MovingCamera camera = MovingCamera(glm::radians(100.0f),WIDTH,HEIGHT);
    camera.getTransform()->translate(glm::vec3(0.0f,0.5f,-10.0f));

    GLuint vao;
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    // Create a visualization method for the particle system below.
    BillboardParticleRenderer renderer { "share/textures/fire.png", 0.20 };
    const ShaderProgram& rendererProgram = renderer.getProgram();

    // Create an example Backwake scenario.
    BackwakeScenario backwakeScenario(32,32,32);
    backwakeScenario.generate();

    // Create the particle system which will compute step.
    ParticleSystem system = ParticleSystem(100000, 20.0f);
    // Add a single emitter which will spawn particles into the scenario.
    system.addEmitter(glm::vec3(0.0f,0.0f,0.0f), glm::vec3( 32.0f,2.0f,32.0f));

    // Initialize with the correct computing accelerator.
    // Usually: Intel, NVIDEA or AMD are platform vendors.
    system.init("share/kernels/particles.cl", "particles",
                "NVIDEA", rendererProgram);
    // Finally, assign the scenario to it.
    system.setScenario(backwakeScenario);

    // Keep track of slowdown/speedup.
    float currentTime = glfwGetTime();
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    float accumulatedTime = 0.0f;

    // For counting the framerate.
    unsigned accumulatedFrames = 0;
    float frameAccumulatedTime = 0.0;

    // We have arrived at the main loop!
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

        // Step the particle simulation time forward:
        if (Locator::input()->isKeyPressed(GLFW_KEY_UP))
            system.compute(accumulatedTime, deltaTime);
        if (Locator::input()->isKeyPressed(GLFW_KEY_DOWN))
            system.compute(accumulatedTime, -deltaTime);

        // // Compute the next step in the particle simpulation
        // // where we issue each step in parallel using a GPU.
        // system.compute(accumulatedTime, deltaTime);

        // Clear the relevant OpenGL buffers each update.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Move with the camera. Yea!
        camera.handleInput(deltaTime);

        glBindVertexArray(vao);
        // Finally, draw the simulated particles.
        renderer.draw(system, camera, currentTime);
        glBindVertexArray(0);

        // Swap the render buffer to display
        glfwSwapBuffers(window);
    }

    // Exit was ok!
    glfwTerminate();
    return 0;
}
