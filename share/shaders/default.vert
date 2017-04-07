#version 410 core
  
layout (location = 0) in vec3 position;

uniform mat4 MVP;

out vec3 vertex_position;

void main()
{
	vertex_position = position;
    gl_Position  = MVP * vec4(position.x, position.y, position.z, 1.0);
}