#ifndef MOVING_CAMERA_HPP
#define MOVING_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Transform.hpp"

class MovingCamera {
private:
  glm::mat4 _projection;
  Transform* _transform;
  float _width;
  float _height;
  bool _mouseLocked;
  glm::vec2 _lastMousePos;
  const float SPEED = 1.0f;
  const float SPEED_MOD = 3.0f;
public:
  MovingCamera() = delete;
  ~MovingCamera();

  MovingCamera(const float& fov, const float& width, const float& height);

  void handleInput(const float& delta);
  void update(const GLuint& programId);
  
  Transform* getTransform();
  glm::mat4 getViewProjection();
  glm::mat4 getView();
  glm::mat4 getMVP();

};
#endif // MOVING_CAMERA_HPP
