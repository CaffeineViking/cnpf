#include "Locator.hpp"
#include <iostream>
#include <utility>
#include "OpenCLUtils.hpp"
#include "foreign/anttweakbar.h"

// Locator for naughty developers
const glm::vec2 DummyInputLocator::getMousePos(){
	return glm::vec2(0.0f,0.0f);
}

// Unused since we don't want a response
bool DummyInputLocator::isButtonPressed(int button){
	(void)button;
	return false;
}

// Unused since we don't want a response
bool DummyInputLocator::isKeyPressed(int key){
	(void)key;
	return false;
}

std::map<int,bool> GLFWInputLocator::_keys;
std::map<int,bool> GLFWInputLocator::_buttons;
glm::vec2 GLFWInputLocator::_mousePos = glm::vec2(0.0f,0.0f);
GLFWwindow* GLFWInputLocator::_window;

void GLFWInputLocator::cursor_callback(GLFWwindow* window, double x, double y)
{

	if(!TwEventMousePosGLFW(x,y)){
		// Supress unised warning
		(void)window;
		_mousePos  = glm::vec2(x,y);
	}
}

void GLFWInputLocator::mouse_callback(GLFWwindow* window, int button, int action, int mods){

	_window = window;
	(void)mods;
	if(!TwEventMouseButtonGLFW(button, action)){
		if(action == GLFW_RELEASE){
			_buttons[button] = false;
		}else if(action == GLFW_PRESS){
			_buttons[button] = true;
		}
	}

	// Supress unised warning


}

void GLFWInputLocator::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

  if(!TwEventKeyGLFW(key, TW_KMOD_NONE) && !TwEventCharGLFW(key, action)){
	// Supress unised warning
	(void)scancode;
	(void)mode;
	if(action == GLFW_RELEASE){
		_keys[key] = false;
	}else if(action == GLFW_PRESS){
		_keys[key] = true;
	}

	if ((key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
         || (key == GLFW_KEY_Q && action == GLFW_PRESS))
		glfwSetWindowShouldClose(window, GL_TRUE);
}
}



const glm::vec2 GLFWInputLocator::getMousePos(){
	return _mousePos;
}

bool GLFWInputLocator::isButtonPressed(int button){
	try
	{
		return _buttons.at(button);
	}
	catch (std::out_of_range& e)
	{
		return false;
	}
	return false;
}

bool GLFWInputLocator::isKeyPressed(int key){
	try
	{
		return _keys.at(key);
	}
	catch (std::out_of_range& e)
	{
		return false;
	}
	return false;
}

GLFWwindow* GLFWInputLocator::getWindow(){
  return _window;
}
InputLocator* Locator::_inputLocator = nullptr;
DummyInputLocator Locator::_dummyInput;

InputLocator* Locator::input()
{
	if(_inputLocator != nullptr)
		return _inputLocator;
	else
		return &_dummyInput;
} 

void Locator::setInput(InputLocator* input){
	_inputLocator= input;
}


GPULocator::GPULocator(std::vector<std::string> kernels, std::vector<std::string> paths, std::string device): _paths{paths}
{
	clParameters params = OpenCLUtils::initCL(kernels, paths, device);
	_device = params.device;
	_program = params.program;
	_queue = params.queue;
	_context = params.context;
	_kernels = params.kernels;
}

cl::Context GPULocator::getContext(){
	return _context;
}

cl::CommandQueue GPULocator::getQueue(){
	return _queue;
}

cl::Kernel GPULocator::getKernel(const std::string& name){
	return _kernels.at(name);
}

cl::Program GPULocator::getProgram(){
	return _program;
}

cl::Device GPULocator::getDevice(){
	return _device;
}