#include "ShaderProgram.hpp"
#include <iostream>
#include "glm/glm.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

ShaderProgram::~ShaderProgram(){
	glDeleteProgram(_programId);
	_programId = 0;
}

ShaderProgram::ShaderProgram(){
	_programId = glCreateProgram();
}

void ShaderProgram::attach(const Shader&shader){
	glAttachShader(_programId, shader.getID());
	link();
}

bool ShaderProgram::link(){
	glLinkProgram(_programId);
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

void ShaderProgram::begin(){
	glUseProgram(_programId);
}

void ShaderProgram::end(){
	glUseProgram(0);
}
GLuint ShaderProgram::getId(){
	return _programId;
}

