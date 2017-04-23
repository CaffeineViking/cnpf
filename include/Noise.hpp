#ifndef NOISE_HPP
#define NOISE_HPP

#include <vector>
#include <utility>
#include "simplexnoise1234.hpp"
#include <glm/gtc/noise.hpp>
#include <glm/glm.hpp>

class Noise {
private:
	std::vector<std::pair<glm::vec3,glm::vec3>> _params;
public:
	void addNoise(const glm::vec3&,const glm::vec3&);
	void cleanNoise();
	float get(const glm::vec3&) const;
};
#endif