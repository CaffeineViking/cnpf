#ifndef MOVING_CAMERA_HPP
#define MOVING_CAMERA_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "Transform.hpp"

constexpr float PI = 3.1415;
constexpr float HALF_PI = PI / 2;

class MovingCamera {
private:
    glm::mat4 _projection;
    Transform* _transform;
    float _width;
    float _height;
    bool _mouseLocked;
    glm::vec2 _lastMousePos;
    const float SPEED = 1.0f;
    const float SPEED_MOD = 8.0f;

    bool pitchPossible(const float& pitch);
public:
    MovingCamera() = delete;
    ~MovingCamera();

    MovingCamera(const float& fov, const float& width, const float& height);

    void updateProjection(const float, const float, const float);
    void handleInput(const float& delta);
    void update(const GLuint& programId) const;

    Transform* getTransform();
    const Transform* getTransform() const;
    glm::mat4 getViewProjection() const;
    glm::mat4 getView() const;
    glm::mat4 getMVP() const;
};

#endif // MOVING_CAMERA_HPP
