#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <CL/cl.hpp>
#include <glm/glm.hpp>
#include "OpenCLUtils.hpp"
#include "OpenGLUtils.hpp"
#include "ShaderProgram.hpp"
#include "VectorField3D.hpp"
#include "Scenario.hpp"
#include <vector>
#include <utility>
class ParticleSystem {
private:
	 const int PARTICLE_COUNT;
	 float _maxParticleCount;
	 float _maxTime;
	 float _respawnTime;
	 float _fieldMagnitude;
	 float _noiseRatio;
	 glm::vec3 _fieldDirection;
	 int _width, _height, _depth;
	 cl::ImageGL _texture;
	 clParameters _params;
	 cl::BufferGL _tmp;
     GLuint _vertexBufferId;
	 cl::Buffer _vertexBuffer;
	 cl::Buffer _timerBuffer;
	 cl::Buffer _spheresBuffer;

	std::string readKernelFile(const std::string&);
	std::vector<std::pair<glm::vec3, glm::vec3>> _emitters;
	std::vector<float> _spheres;
public:	
	~ParticleSystem();
	ParticleSystem(const int,const float);

	bool init(std::vector<std::string>, std::vector<std::string>, const std::string&, const ShaderProgram&);
	bool setScenario(const Scenario&);
	void compute(const float, const float);
	void addSphere(const glm::vec3&, const float);
	void addEmitter(const glm::vec3&, const glm::vec3&);
	int getParticleCount(const float) const;
	float* referenceRespawnTime();
	float* referenceFieldMagnitude();
	float* referenceNoiseRatio();
	glm::vec3* referenceFieldDirection();
	ParticleSystem(const ParticleSystem&) = default;
	ParticleSystem& operator=(const ParticleSystem&) = default;

	ParticleSystem(ParticleSystem&&) = default;
	ParticleSystem& operator=(ParticleSystem&&) = default;
	float* getMaxParticleCount();
};
#endif
