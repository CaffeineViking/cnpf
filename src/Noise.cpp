#include "Noise.hpp"

void Noise::addNoise(const glm::vec3& offset,const glm::vec3& scale) {
	_params.push_back(std::make_pair(offset,scale));
}

void Noise::cleanNoise() {
	_params.clear();
}

float Noise::get(const glm::vec3& position) const{
	float result = 0.0f;
	for(std::pair<glm::vec3,glm::vec3> p: _params){
		float noise = glm::perlin(glm::vec3(
			position.x * p.second.x + p.first.x,
			position.y * p.second.y + p.first.y,
			position.z * p.second.z + p.first.z));
		result += noise;
	}

	return result / ((float)_params.size());
}