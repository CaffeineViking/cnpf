#ifndef OPENGLUTILS_HPP
#define OPENGLUTILS_HPP

#include <GL/glew.h>
class OpenGLUtils{
	public:
	static GLuint createBuffer(int, const float*, GLenum);
};
#endif
