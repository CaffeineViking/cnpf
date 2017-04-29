#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//==================================================================
// Simple wrapper around a model-view projection.
// Apply liberally to burned or sore areas within a shader program.
//==================================================================
class Camera{
private:
	glm::vec3 _cameraPosition;
	glm::vec3 _cameraTarget;
	glm::mat4 _projection;
	glm::mat4 _view;
	glm::mat4 _model;
	float _angle;
	float _FOV;
	float _width;
	float _height;
	glm::mat4 _mvp;
public:	
	Camera(const Camera&) = default;
	Camera& operator=(const Camera&) = default;

	Camera(Camera&& shape) = default;
	Camera& operator=(Camera&&) = default;

	Camera() = delete;
	~Camera() = default;

	Camera(float, float, float);
	void rotate(float);
	void translate(const glm::vec3&);
	void lookAt(const glm::vec3);
	void update(const GLuint&);
};
#endif