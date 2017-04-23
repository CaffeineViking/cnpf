#version 410 core

in vec3 vertex_position;
out vec4 color;

void main()
{	
	float d = distance(vertex_position,vec3(0.0,0.0,0.0));
	if (d < 4) {
		color = vec4(1.0,0.5,0.5,1.0);
	}
	else {
		color = vec4(0.5,0.5,0.5,0.2);
	}
} 