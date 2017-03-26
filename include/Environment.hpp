#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include <vector>
#include "Light.hpp"
class Environment {
private:
//	std::vector<const Light&> _lights;
public:
	Environment() = default;

	void addLight(const Light&);
	void removeLight(const Light&);

	void begin(const GLuint&);
	void end();
};
#endif