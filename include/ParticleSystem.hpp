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

typedef struct Params {
  float width;
  float height;
  float depth;
  float fieldMagnitude;
  float noiseRatio;
  float lengthScale;
  float noiseMagnitude;
  float boundraryWidth;
  glm::vec3 fieldDirection;
} Params;

#define MAX_POSITIONS_BUFFERS 8

class ParticleSystem {
private:
	 const int PARTICLE_COUNT;
	 float _maxParticleCount;
	 float _currentParticles;
	 float _particlePerFrame;
	 float _maxTime;
	 float _respawnTime;
	 float _fieldMagnitude;
	 float _noiseRatio;
	 float _noiseMagnitude;
	 float _lengthScale;
	 glm::vec3 _fieldDirection;
	 int _width, _height, _depth;
     int _positionsBufferSize, _positionsBufferHead;
	 cl::ImageGL _texture;
	 cl::Image2D _outputImage;
	 clParameters _params;
	 cl::BufferGL _tmp;
     cl::BufferGL _positionsGLBuffer[MAX_POSITIONS_BUFFERS];
     GLuint _vertexBufferId;
     GLuint _positionsBufferId[MAX_POSITIONS_BUFFERS];
	 cl::Buffer _vertexBuffer;
	 cl::Buffer _timerBuffer;
	 cl::Buffer _spheresBuffer;
     cl::Buffer _positionsBuffer;
     cl::Buffer _spawnerBuffer;

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
	float* referenceParticlesPerFrame();
	float* referenceLengthScale();
	float* referenceNoiseMagnitude();
	glm::vec3* referenceFieldDirection();

	bool snapshot(const std::string&, const std::string&);
	ParticleSystem(const ParticleSystem&) = default;
	ParticleSystem& operator=(const ParticleSystem&) = default;

	ParticleSystem(ParticleSystem&&) = default;
	ParticleSystem& operator=(ParticleSystem&&) = default;
	float* getMaxParticleCount();
};
#endif
