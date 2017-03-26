#include "Shape.hpp"


Plane::~Plane(){
     glDeleteBuffers(1, &_id);
     glDeleteBuffers(1, &_uvId);
}

Plane::Plane(float width, float height): Shape(), _width{width}, _height{height}
{

    float hw = width/2.0f;
    float hh = height/2.0f;
    const GLfloat vertexBuffer[] = {
      -hw, -hh, 0.0f,
       hw, -hh, 0.0f,
      -hw,  hh, 0.0f,
       hw,  hh, 0.0f
    };

    glGenBuffers(1, &_id);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);

    const GLfloat uvBuffer[] = {
        0.0f, 1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f
    };
    glGenBuffers(1, &_uvId);
    glBindBuffer(GL_ARRAY_BUFFER, _uvId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvBuffer), uvBuffer, GL_STATIC_DRAW);
}

void Plane::render(){

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _uvId);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0,4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


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