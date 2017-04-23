#ifndef OPENGLUTILS_HPP
#define OPENGLUTILS_HPP

#include <GL/glew.h>
#include <vector>
#include <string>
class OpenGLUtils{
	public:
	static GLuint createBuffer(int, const float*, GLenum);
	static GLuint createTexture(unsigned,unsigned,const float*);
	static GLuint createTexture3D(unsigned ,unsigned ,unsigned , const float* );
	static bool writePNG(const std::string&, const unsigned ,const unsigned, const std::vector<float>& );
	static bool loadPNG(const std::string&, unsigned&, unsigned&, std::vector<float>&);
	static void bufferToPNG_X(const std::string&, const std::vector<float>&, int, int, int);
	static void bufferToPNG_Y(const std::string&, const std::vector<float>&, int, int, int);

};
#endif
