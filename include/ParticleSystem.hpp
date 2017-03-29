#ifndef PARTICLESYSTEM_HPP
#define PARTICLESYSTEM_HPP

#include <string>
#include <fstream>
#include <CL/cl.hpp>
class ParticleSystem {
private:
	 int _numParticles;
	 cl::Device _gpuDevice;
 	 cl::Context _clContext;
     cl::Program::Sources _clSources;
	 cl::Program _clProgram;

	std::string readKernelFile(const std::string&);
public:	
	~ParticleSystem();
	ParticleSystem();

	bool init(const std::string&);
	void compute();

	ParticleSystem(const ParticleSystem&) = default;
	ParticleSystem& operator=(const ParticleSystem&) = default;

	ParticleSystem(ParticleSystem&&) = default;
	ParticleSystem& operator=(ParticleSystem&&) = default;
};
#endif