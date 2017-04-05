#version 410 core

in vec3 vertex_position;
out vec4 color;

void main()
{	
    color = vec4(abs(normalize(vertex_position)),0.5);
} 