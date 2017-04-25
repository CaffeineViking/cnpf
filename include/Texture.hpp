#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Texture{
private:
	GLuint _id;
	unsigned _width;
	unsigned _height;
	bool _valid;

public:
	~Texture();
	Texture() : _id { 0xDEADB33F}, _width {0}, _height{0}, _valid{false} {}
	Texture(const unsigned, const unsigned, const float*);

	Texture(Texture&&);
	Texture& operator=(Texture&&);
	GLuint getId() const { return _id; }

	void begin(int);
	void end();
};

#endif
