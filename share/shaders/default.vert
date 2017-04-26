#version 410 core

layout (location = 0) in vec3 position;

uniform mat4 view;

out vec3 vertex_position;

void main()
{
	mat4 translationMatrix = mat4(1);
	mat4 rotationMatrix = mat4(1);
	mat4 scaleMatrix = mat4(1);

	translationMatrix[3][0] = position.x;
	translationMatrix[3][1] = position.y;
	translationMatrix[3][2] = position.z;

	// // Scale, rotate and then translate.
	// // The order is important
	mat4 model = translationMatrix * rotationMatrix * scaleMatrix;

	vertex_position = position;
	gl_Position  = view * model * vec4(position.x, position.y, position.z, 1.0);
}
