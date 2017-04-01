#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <string>
#include <fstream>
#include <CL/cl.hpp>
#include <glm/glm.hpp>
#include "OpenCLUtils.hpp"
#include "OpenGLUtils.hpp"
#include "ShaderProgram.hpp"
#include <vector>
#include <utility>
class ParticleSystem {
private:
	 const int PARTICLE_COUNT;
	 float maxTime;
	 clParameters _params;
	 cl::BufferGL _tmp;
	 cl::Buffer _vertexBuffer;
	 cl::Buffer _velocityBuffer;
	 cl::ImageGL _texture;
	 unsigned _width,_height;
	std::string readKernelFile(const std::string&);
	std::vector<std::pair<glm::vec3, glm::vec3>> _emitters;
public:	
	~ParticleSystem();
	ParticleSystem(const int,const float);

	bool init(const std::string&, const std::string&, const std::string&, const ShaderProgram&);
	void compute(const float);
	void addEmitter(const glm::vec3&, const glm::vec3&);
	int getParticleCount(const float) const;
	ParticleSystem(const ParticleSystem&) = default;
	ParticleSystem& operator=(const ParticleSystem&) = default;

	ParticleSystem(ParticleSystem&&) = default;
	ParticleSystem& operator=(ParticleSystem&&) = default;

};
#endif