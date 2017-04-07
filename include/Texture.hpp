#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
class Texture{
private:
	GLuint _id;
	const int _width;
	const int _height;
public:
	~Texture();
	Texture();
	Texture(const int, const int, const float*);

	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;

	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;

	void begin();
	void end();
};
#endif