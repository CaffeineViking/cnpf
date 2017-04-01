#ifndef OPENGLUTILS_HPP
#define OPENGLUTILS_HPP

#include <GL/glew.h>
#include <vector>
#include <string>
class OpenGLUtils{
	public:
	static GLuint createBuffer(int, const float*, GLenum);
	static GLuint createTexture(unsigned,unsigned,const float*);
	static bool loadPNG(const std::string&, unsigned&, unsigned&, std::vector<float>&);
};
#endif
