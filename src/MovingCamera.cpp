#include "MovingCamera.hpp"
#include "Locator.hpp"
#include <iostream>
#include <glm/ext.hpp>

MovingCamera::MovingCamera(const float& fov, const float& width, const float& height)
  : _width(width), _height(height), _mouseLocked(false)
{
  _projection = glm::perspective(fov,
				 (float)width/(float)height,
				 0.1f,
				 2200.0f);
  _transform = new Transform();
}

MovingCamera::~MovingCamera(){
  delete _transform;
}

void MovingCamera::handleInput(const float& delta){
  // unused for the moment
  (void)delta;
  
  float sensitivity = 0.001f;
  int lockMouseKey = GLFW_MOUSE_BUTTON_LEFT;
  
  glm::vec2 center(_width/2.0f, _height/2.0f);

  GLFWInputLocator* input = dynamic_cast<GLFWInputLocator*>(Locator::input());
  //input->setInput(new GLFWInputLocator());
  GLFWwindow* window = input->getWindow();

  //============================
  // Rotate camera
  //============================
  if(!_mouseLocked && input->isButtonPressed(lockMouseKey))
    {
      _mouseLocked = true;
      _lastMousePos = input->getMousePos();
      if(window)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

  if(!input->isButtonPressed(lockMouseKey))
    {
      _mouseLocked = false;
      if(window)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  
  if(_mouseLocked){
    glm::vec2 mousePos = input->getMousePos();
    
    float rotX = mousePos.x - _lastMousePos.x;
    float rotY = mousePos.y - _lastMousePos.y;
    
    _lastMousePos = input->getMousePos();

    //std::cout << "pitch: " << rotY*sensitivity << "\tyaw: " << rotX*sensitivity << std::endl;

    // Rotate around local x-axis (vector pointing to the right of the object)
    if(rotY != 0)
      getTransform()->rotate(rotY * sensitivity, getTransform()->getRight());

    // Rotate around y-axis
    if(rotX != 0)
      getTransform()->rotate((-rotX) * sensitivity, glm::vec3(0,1,0));
  }

  //============================
  // Move camera
  //============================
  glm::vec3 translation(0.0f);
  float amount = delta * SPEED;
  
  if(input->isKeyPressed(GLFW_KEY_W))
    translation += getTransform()->getForward() * amount;

  if(input->isKeyPressed(GLFW_KEY_S))
    translation += getTransform()->getBackward() * amount;
  
  if(input->isKeyPressed(GLFW_KEY_A))
    translation += getTransform()->getRight() * amount;
  
  if(input->isKeyPressed(GLFW_KEY_D))
    translation += getTransform()->getLeft() * amount;
  
  if(input->isKeyPressed(GLFW_KEY_SPACE))
    translation += getTransform()->getUp() * amount;
  
  if(input->isKeyPressed(GLFW_KEY_LEFT_CONTROL))
    translation += getTransform()->getDown() * amount;

  if(input->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
    translation *= SPEED_MOD;
  
  getTransform()->translate(translation);
  
  
}

Transform* MovingCamera::getTransform(){
  return _transform;
}

glm::mat4 MovingCamera::getViewProjection(){
  glm::vec3 eye = _transform->getPos();
  glm::vec3 center = _transform->getPos() + _transform->getForward();
  glm::vec3 up = _transform->getUp();
  return _projection * glm::lookAt(eye, center, up);
}

glm::mat4 MovingCamera::getMVP() {
  return getViewProjection() * getTransform()->getModel();;
}

void MovingCamera::update(const GLuint& programId){
  //glm::mat4 mvp = getMVP();
  glm::mat4 vp = getViewProjection();

  GLuint vpId = glGetUniformLocation(programId, "viewProjection");
  glUniformMatrix4fv(vpId, 1, GL_FALSE, &(vp[0][0]));
}
