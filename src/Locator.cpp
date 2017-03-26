#include "Locator.hpp"
#include <iostream>
#include <utility>
// Locator for naughty developers
const glm::vec2 DummyInputLocator::getMousePos(){
	return glm::vec2(0.0f,0.0f);
}

bool DummyInputLocator::isButtonPressed(int button){
	return false;
}

bool DummyInputLocator::isKeyPressed(int key){
	return false;
}

std::map<int,bool> GLFWInputLocator::_keys;
std::map<int,bool> GLFWInputLocator::_buttons;
glm::vec2 GLFWInputLocator::_mousePos = glm::vec2(0.0f,0.0f);

void GLFWInputLocator::cursor_callback(GLFWwindow* window, double x, double y)
{
	_mousePos  = glm::vec2(x,y);
}

void GLFWInputLocator::mouse_callback(GLFWwindow* window, int button, int action, int mods){
	if(action == GLFW_RELEASE){
		_buttons[button] = false;
	}else if(action == GLFW_PRESS){
		_buttons[button] = true;
	}

}

void GLFWInputLocator::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(action == GLFW_RELEASE){
		_keys[key] = false;
	}else if(action == GLFW_PRESS){
		_keys[key] = true;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
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