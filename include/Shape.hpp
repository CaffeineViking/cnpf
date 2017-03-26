#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#ifdef WINDOWS
#include <windows.h>
#endif

//==============================================================================================
// Super simple class-system for shapes. Ideally used to define vertecies and geometry at once.
//==============================================================================================
class Shape{
protected:
	Shape(const Shape&) = default;
	Shape& operator=(const Shape&) = default;

	Shape(Shape&& shape) = default;
	Shape& operator=(Shape&&) = default;

	Shape() = default;
	~Shape() = default;

public:

	virtual void render() = 0;
};

class Cube: public Shape{
private:
	float _width;
	float _height;
	float _depth;
	GLuint _id; 
public:
	Cube(float, float, float);
	void render();
};
#endif
