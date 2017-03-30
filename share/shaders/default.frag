#version 410 core

in vec3 vertex_position;
out vec4 color;

void main()
{	

    color =vec4(0.0,vertex_position.y*2, 0.2,1.0);
} 