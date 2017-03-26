#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP
#include "Shader.hpp"
#include "Camera.hpp"
//===========================================================================
// Manages shaders akin to typical Shader Programs such as the one in LibGDX
// Tries to not limit to only fragment and vertex shaders
//===========================================================================
class ShaderProgram{
private:
	GLuint _programId;
public:
	~ShaderProgram();
	ShaderProgram();

	ShaderProgram(const ShaderProgram&) = default;
	ShaderProgram& operator=(const ShaderProgram&) = default;

	ShaderProgram(ShaderProgram&&) = default;
	ShaderProgram& operator=(ShaderProgram&&) = default;

	void attach(const Shader&);
	bool link();
	void begin();
	void end();
	const GLuint getId();
};
#endif