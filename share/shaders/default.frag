#version 410 core

in vec3 vertex_position;
out vec4 color;

void main()
{	
    color =vec4(abs(vertex_position.x+0.1),abs(vertex_position.x),abs(vertex_position.z),1.0);
} 