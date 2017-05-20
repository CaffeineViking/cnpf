#include "MovingCamera.hpp"
#include "Locator.hpp"
#include <iostream>
#include <glm/ext.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <cassert>

MovingCamera::MovingCamera(const float& fov, const float& width, const float& height)
    : _width(width), _height(height), _mouseLocked(false), _transitioning(false)
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

void MovingCamera::handleInput(const float& delta, const float time){
    float SENS = 0.001f;
    int lockMouseKey = GLFW_MOUSE_BUTTON_LEFT;

    GLFWInputLocator* input = dynamic_cast<GLFWInputLocator*>(Locator::input());
    assert(input != nullptr);
    GLFWwindow* window = input->getWindow();

	if (_transitioning){
			// set CAMERA_SPEED to value between 0.0 and 1.0
			const float CAMERA_SPEED = 0.5; 
			glm::vec3 translation(0.0f);
			float timeElapsed = time - _startTime;
			// To get a smooth camera movement, smoothFunction is an ease function with values 0->1
			float smoothStep = smoothFunction(timeElapsed*CAMERA_SPEED);
			// Vector addition
			getTransform()->setPos(smoothStep*(_targetPos-_startPos)+ _startPos);
			// This need to be interpolated as well!
			getTransform()->lookAt(_targetLookAtPoint, _upVector);
			
			if (timeElapsed > 1.0f/CAMERA_SPEED)
				_transitioning = false;
	}
	else {
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

		float yaw = SENS * (mousePos.x - _lastMousePos.x);
		float pitch = SENS * (mousePos.y - _lastMousePos.y);
		
			_lastMousePos = input->getMousePos();

			// Rotate around local x-axis (vector pointing to the right of the object)
		if(pitch != 0) {
			if(pitchPossible(pitch))
			getTransform()->rotate(pitch, getTransform()->getRight());
		}
		// Rotate around y-axis
		if(yaw != 0)
			getTransform()->rotate((-yaw), glm::vec3(0,1,0));
		}

		//============================
		// Move camera
		//============================
		glm::vec3 translation(0.0f);
		float amount = delta * SPEED;

		
		if(input->isKeyPressed(GLFW_KEY_1)){
			getTransform()->setPos(glm::vec3(0.0, 0.0, 0.0));
			}
		if(input->isKeyPressed(GLFW_KEY_2)){
			_startPos = getTransform()->getPos();
			_targetPos = glm::vec3(-50.0, 30.0, -50.0);
			_targetLookAtPoint = glm::vec3(0.0, -30.0, 0.0), 
			_upVector = glm::vec3 (0.0, 1.0, 0.0);
			_startTime = time;
			_transitioning = true;
		}
		if(input->isKeyPressed(GLFW_KEY_3)){
			_startPos = getTransform()->getPos();
			_targetPos = glm::vec3(0.0, 0.0, -80.0);
			_targetLookAtPoint = glm::vec3(0.0, 0.0, 0.0), 
			_upVector = glm::vec3 (0.0, 1.0, 0.0);
			_startTime = time;
			_transitioning = true;
		}
		if(input->isKeyPressed(GLFW_KEY_4)){
			_startPos = getTransform()->getPos();
			_targetPos = glm::vec3(0.0, 80.0, 0.0);
			_targetLookAtPoint = glm::vec3(0.0, 0.0, 0.0), 
			_upVector = glm::vec3 (0.0, 00.0, 1.0);
			_startTime = time;
			_transitioning = true;
		}
		
		if(input->isKeyPressed(GLFW_KEY_W))
			translation += getTransform()->getForward() * amount;

		if(input->isKeyPressed(GLFW_KEY_S))
			translation += getTransform()->getBackward() * amount;

		if(input->isKeyPressed(GLFW_KEY_A))
			translation += getTransform()->getRight() * amount;

		if(input->isKeyPressed(GLFW_KEY_D))
			translation += getTransform()->getLeft() * amount;

		if(input->isKeyPressed(GLFW_KEY_SPACE))
		translation += glm::vec3(0,1,0)  * amount;

		if(input->isKeyPressed(GLFW_KEY_LEFT_CONTROL))
		translation += glm::vec3(0,-1,0) * amount;

		if(input->isKeyPressed(GLFW_KEY_LEFT_SHIFT))
			translation *= SPEED_MOD;

		getTransform()->translate(translation);
		if(input->isKeyPressed(GLFW_KEY_P))
			std::cout << getTransform()->getPos().x << " " << getTransform()->getPos().y << " " << getTransform()->getPos().z << std::endl;
			
	}
}

Transform* MovingCamera::getTransform(){
    return _transform;
}

const Transform* MovingCamera::getTransform() const {
    return _transform;
}

void MovingCamera::updateProjection(const float fov, const float width, const float height) {
    _projection = glm::perspective(fov,
            (float)width/(float)height,
            0.1f,
            2200.0f);
}

glm::mat4 MovingCamera::getViewProjection() const {
    glm::vec3 eye = _transform->getPos();
    glm::vec3 center = _transform->getPos() + _transform->getForward();
    glm::vec3 up = _transform->getUp();
    return _projection * glm::lookAt(eye, center, up);
}

glm::mat4 MovingCamera::getView() const {
    glm::vec3 eye = _transform->getPos();
    glm::vec3 center = _transform->getPos() + _transform->getForward();
    glm::vec3 up = _transform->getUp();
    return glm::lookAt(eye, center, up);
}

glm::mat4 MovingCamera::getMVP() const {
    return getViewProjection() * getTransform()->getModel();;
}

void MovingCamera::update(const GLuint& programId) const{
    glm::mat4 v = getView();
    GLuint vId = glGetUniformLocation(programId, "view");
    glUniformMatrix4fv(vId, 1, GL_FALSE, &(v[0][0]));

    glm::mat4 p = _projection;
    GLuint pId = glGetUniformLocation(programId, "projection");
    glUniformMatrix4fv(pId, 1, GL_FALSE, &(p[0][0]));

}

bool MovingCamera::pitchPossible(const float& pitch) {
    glm::vec3 forward = getTransform()->getForward();
    glm::vec3 right = getTransform()->getRight();
    glm::vec3 projForward = glm::vec3(forward.x, 0, forward.z);
    float angle = glm::orientedAngle(forward, projForward, right);

    return angle * pitch >= 0 || fabs(angle) + fabs(pitch) < HALF_PI;
}
// Added for debug
void MovingCamera::printVec3(glm::vec3 vector3){
	std::cout << "Vec3:\nx = " << vector3.x << "   y = " << vector3.y << "   z = " << vector3.z << std::endl;
}

float MovingCamera::smoothFunction(float t){
	  //Ease in and out
	  return t*t*(3.0f - 2.0f * t);
	 //return 3*(1 - t)*(1 - t)*t + 0.4;
}