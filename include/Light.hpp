#ifndef LIGHT_HPP
#define LIGHT_HPP
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>

struct LightParams{
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class Light{
protected:
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;
	float _diffusePower;
	float _specularPower;
public:	
	virtual void render(const GLuint&) = 0; 
};

class PointLight: public Light{
private:
	glm::vec3 _position;
public:
	void render(const GLuint&);
};

#endif