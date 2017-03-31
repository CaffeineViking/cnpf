#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <string>
#include <fstream>
#include <CL/cl.hpp>
#include "OpenCLUtils.hpp"
#include "OpenGLUtils.hpp"
#include "ShaderProgram.hpp"
class ParticleSystem {
private:
	 const int PARTICLE_COUNT;
	 clParameters _params;
	 cl::BufferGL _tmp;
	 cl::Buffer _vertexBuffer;
	 cl::Buffer _velocityBuffer;
	std::string readKernelFile(const std::string&);
public:	
	~ParticleSystem();
	ParticleSystem(const int);

	bool init(const std::string&, const std::string&, const std::string&, const ShaderProgram&);
	void compute(const float);
	int getParticleCount() const;
	ParticleSystem(const ParticleSystem&) = default;
	ParticleSystem& operator=(const ParticleSystem&) = default;

	ParticleSystem(ParticleSystem&&) = default;
	ParticleSystem& operator=(ParticleSystem&&) = default;

};
#endif