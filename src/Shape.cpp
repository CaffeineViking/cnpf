#include "Shape.hpp"

Cube::Cube(float width, float height, float depth): Shape(), _width{width}, _height{height}, _depth{depth}
{
	GLuint VertexArrayID;
	glGenVertexArrays(1,&VertexArrayID);
	glBindVertexArray(VertexArrayID);

	float hw = width/2.0f;
	float hh = height/2.0f;
	float hd = depth/2.0f;
	const GLfloat vertexBuffer[] = {
    -hw,-hh,-hd,
    -hw,-hh, hd,
    -hw, hh, hd, 
     hw, hh,-hd,
    -hw,-hh,-hd,
    -hw, hh,-hd, 
     hw,-hh, hd,
    -hw,-hh,-hd,
     hw,-hh,-hd,
     hw, hh,-hd,
     hw,-hh,-hd,
    -hw,-hh,-hd,
    -hw,-hh,-hd,
    -hw, hh, hd,
    -hw, hh,-hd,
     hw,-hh, hd,
    -hw,-hh, hd,
    -hw,-hh,-hd,
    -hw, hh, hd,
    -hw,-hh, hd,
     hw,-hh, hd,
     hw, hh, hd,
     hw,-hh,-hd,
     hw, hh,-hd,
     hw,-hh,-hd,
     hw, hh, hd,
     hw,-hh, hd,
     hw, hh, hd,
     hw, hh,-hd,
    -hw, hh,-hd,
     hw, hh, hd,
    -hw, hh,-hd,
    -hw, hh, hd,
     hw, hh, hd,
    -hw, hh, hd,
     hw,-hh, hd
	};

	glGenBuffers(1, &_id);
	glBindBuffer(GL_ARRAY_BUFFER, _id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
}

void Cube::render(){
  	glBindVertexArray(_id);
    glDrawArrays(GL_TRIANGLE_STRIP, 0,36);
    glBindVertexArray(0);
}