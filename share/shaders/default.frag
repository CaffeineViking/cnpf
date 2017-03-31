#version 410 core

in vec3 vertex_position;
out vec4 color;

void main()
{	
    color =vec4(clamp(abs(vertex_position),0,1),1.0);
} 