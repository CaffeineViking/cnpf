#include "Camera.hpp"
#include "Locator.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(float FOV, float width, float height):
_cameraPosition{0.0f,0.0f,0.0f},
_cameraTarget{0.0f,0.0f,0.0f},
_angle{0.0f},
_FOV{FOV},
_width{width},
_height{height}
{
	_projection = glm::perspective(glm::radians(_FOV), (float) _width / (float)_height, 0.1f, 2200.0f);
	_model = glm::mat4(1.0f);
	_view = glm::lookAt(_cameraPosition, _cameraTarget, glm::vec3(0,2,0));
	translate(glm::vec3(0.0f,0.0f,0.0f));
}

void Camera::rotate(float deg){
	glm::vec3 v2 = _cameraPosition - _cameraTarget;
	glm::vec3 v3 = glm::rotate(v2,glm::radians(deg), glm::vec3(0.0f,1.0f,0.0f));
	_cameraPosition = v3 + _cameraTarget;
	translate(glm::vec3(0.0f,0.0f,0.0f));
}

void Camera::translate(const glm::vec3& pos){
	_cameraPosition +=  pos;
	_view = glm::lookAt(_cameraPosition, _cameraTarget, glm::vec3(0,1,0)  );
}

void Camera::lookAt(const glm::vec3 target){
	_cameraTarget = target;
	_view = glm::lookAt(_cameraPosition, _cameraTarget, glm::vec3(0,1,0)  );
}

void Camera::update(const GLuint& programId ){
	
	_mvp = _projection * _view * _model;

	GLuint mvpId = glGetUniformLocation(programId, "MVP");
	glUniformMatrix4fv(mvpId, 1, GL_FALSE, &_mvp[0][0]);

}
