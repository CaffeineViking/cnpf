#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
class Texture{
private:
	GLuint _id;
public:
	~Texture();
	Texture();

	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;

	Texture(Texture&&) = default;
	Texture& operator=(Texture&&) = default;

	void begin();
	void end();
};
#endif