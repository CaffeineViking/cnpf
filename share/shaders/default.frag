#version 410 core

in vec3 vertex_position;
out vec4 color;

void main()
{	
    //color =vec4(normalize(abs(vertex_position)),1.0);
    float v = length(vertex_position);
    color = vec4(v,v,v,0.2);
} 