#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
  Transform(const glm::vec3& pos = glm::vec3(0,0,0), 
	    const glm::quat& rot = glm::quat(1.0f,0.0f,0.0f,0.0f),
	    const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f));
  virtual ~Transform();
  
  glm::mat4 getModel() const;
  glm::mat4 getMVP(const glm::mat4& viewPojection);
  glm::mat4 getMVP(const glm::mat4& viewPojection) const;

  void rotate(const float& angle, const glm::vec3& axis);
  void rotate(glm::quat rotation);
  void translate(const glm::vec3&);

  glm::vec3 getRight()    const;
  glm::vec3 getLeft()     const;
  glm::vec3 getUp()       const;
  glm::vec3 getDown()     const;
  glm::vec3 getForward()  const;
  glm::vec3 getBackward() const;

  inline glm::vec3 getPos()   const { return m_pos; }
  inline glm::quat getRot()   const { return m_rot; }
  inline glm::vec3 getScale() const { return m_scale; }

  inline void setPos(const glm::vec3& pos)     { m_pos = pos; } 
  inline void setRot(const glm::quat& rot)     { m_rot = rot; }
  inline void setScale(const glm::vec3& scale) { m_scale = scale; }
  inline void setParent(Transform* parent)     { m_parent = parent; }

  glm::vec3 getTransformedPos()   const;
  glm::quat getTransformedRot()   const; 
  glm::vec3 getTransformedScale() const;

  void lookAt(glm::vec3 point, glm::vec3 up);
  glm::quat getLookAtRot(glm::vec3 point, glm::vec3 up) const;

  bool hasChanged() const;
  void update();

private:
  glm::vec3 m_pos{ glm::vec3() };
  glm::quat m_rot{ glm::quat() };
  glm::vec3 m_scale{ glm::vec3() };

  glm::vec3 m_oldPos{ glm::vec3(1,1,1) };
  glm::quat m_oldRot{ glm::quat(0,0,0,1) };
  glm::vec3 m_oldScale{ glm::vec3(1,1,1) };

  Transform* m_parent;

  glm::mat4 m_parentMatrix;

  /* ====================================
   * Transform helper functions
   * for vector, matrix and quaternion
   * operations 
   * ==================================== */
  glm::vec3 transform(glm::mat4 m, glm::vec3 r) const;
  glm::vec3 rotate(glm::vec3 v, glm::quat q) const;
  glm::quat mul(glm::quat q, glm::vec3 v) const;
  glm::mat4 getTranslationMatrix(const glm::vec3& v) const;
  glm::mat4 getScaleMatrix(const glm::vec3& v) const;
  glm::mat4 toMat4(const glm::quat& q) const;

  glm::mat4 initRotation(glm::mat4 m, glm::vec3 forward, glm::vec3 up) const;
  glm::mat4 initRotation(glm::mat4 m, glm::vec3 forward, glm::vec3 up, glm::vec3 right) const;
};
#endif // TRANSFORM_H
