#ifndef LOCATOR_HPP
#define LOCATOR_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>
#include <CL/cl.hpp>
#include "glm/glm.hpp"

class InputLocator {
public:
	virtual const glm::vec2 getMousePos() = 0;
	virtual bool isButtonPressed(int) = 0;
	virtual bool isKeyPressed(int) = 0; 
};

class DummyInputLocator: public InputLocator{
public:
	const glm::vec2 getMousePos();
	bool isButtonPressed(int);
	bool isKeyPressed(int); 
};

class GLFWInputLocator: public InputLocator{
private:
	static glm::vec2 _mousePos;
	static std::map<int,bool> _keys;
	static std::map<int,bool> _buttons;
static GLFWwindow* _window;
public:
	// Static because of clash between C and C++
	static void cursor_callback(GLFWwindow*, double, double);
	static void mouse_callback(GLFWwindow*, int, int, int);
	static void keyboard_callback(GLFWwindow*, int, int, int, int);
	const glm::vec2 getMousePos();
	bool isButtonPressed(int);
	bool isKeyPressed(int);
    GLFWwindow* getWindow();
};

class GPULocator {
private:
    cl::Device _device;
    cl::CommandQueue _queue;
    cl::Program _program;
    std::map<std::string,cl::Kernel> _kernels;
    cl::Context _context;
    std::vector<std::string> _paths;
public:
    GPULocator(std::vector<std::string>, std::vector<std::string>, std::string);
    cl::Context getContext();
    cl::CommandQueue getQueue();
    cl::Kernel getKernel(const std::string&);
    cl::Program getProgram();
    cl::Device getDevice();
};


class Locator{
private:
	static DummyInputLocator _dummyInput;
	static InputLocator* _inputLocator;

public:
	static InputLocator* input();
	static void setInput(InputLocator*);
};
#endif
