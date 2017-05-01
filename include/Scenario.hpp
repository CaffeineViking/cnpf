#ifndef SCENARIO_HPP
#define SCENARIO_HPP
#include <GL/glew.h>
#include <CL/cl.hpp>
#include "VectorField3D.hpp"
#include "Noise.hpp"

class Scenario{
protected:
	int _width,_height,_depth;
	VectorField3D _field;
	Scenario(int,int,int);
public:
	virtual void generate() = 0;
	virtual glm::vec3 potential(const glm::vec3&, const Noise&) = 0;
	int getWidth() const;
	int getHeight() const;
	int getDepth() const;
	VectorField3D getField() const;
};

class DebugScenario: public Scenario {
public:
	DebugScenario(int,int,int);
	void generate();
	glm::vec3 potential(const glm::vec3&, const Noise&);
};

class BackwakeScenario: public Scenario {
public:
	BackwakeScenario(int,int,int);
	void generate();
	glm::vec3 potential(const glm::vec3&, const Noise&);
};
#endif