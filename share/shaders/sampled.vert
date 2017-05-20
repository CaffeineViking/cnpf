#version 410 core
#define MAX_POSITIONS_BUFFERS 8

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 oldPosition;
layout (location = 2) in vec3 positionsBuffer[MAX_POSITIONS_BUFFERS];

uniform mat4 view;

out vec3 vertex_position;
out vec4 old_vertex_positions[MAX_POSITIONS_BUFFERS];

void main()
{
    mat4 translationMatrix = mat4(1);
    mat4 rotationMatrix = mat4(1);
    mat4 scaleMatrix = mat4(1);

    vec3 pos = position;
    vertex_position = pos;
    translationMatrix[3][0] = pos.x;
    translationMatrix[3][1] = pos.y;
    translationMatrix[3][2] = pos.z;

    // Scale, rotate and then translate.
    // The order is important
    mat4 model = translationMatrix; //* rotationMatrix * scaleMatrix;
    gl_Position  = view * model * vec4(1);

    for(int i = 0; i < MAX_POSITIONS_BUFFERS; i++){
    	translationMatrix[3][0] = positionsBuffer[i].x;
    	translationMatrix[3][1] = positionsBuffer[i].y;
    	translationMatrix[3][2] = positionsBuffer[i].z;
    	model = translationMatrix; //* rotationMatrix * scaleMatrix;
    	old_vertex_positions[i] = view * model * vec4(1);
    }
}
