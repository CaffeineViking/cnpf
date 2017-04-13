#version 410 core

in vec3 final_vertex_position;
out vec4 color;

void main()
{	
    color = vec4(abs(normalize(final_vertex_position)),0.6);
} 
