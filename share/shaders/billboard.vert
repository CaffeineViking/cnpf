#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 oldPosition;
uniform mat4 view;

out vec3 vertex_position;
out vec4 old_vertex_position;

void main()
{
    mat4 translationMatrix = mat4(1);
    mat4 rotationMatrix = mat4(1);
    mat4 scaleMatrix = mat4(1);

    vec3 pos = position;
    translationMatrix[3][0] = pos.x;
    translationMatrix[3][1] = pos.y;
    translationMatrix[3][2] = pos.z;

    // // Scale, rotate and then translate.
    // // The order is important
    mat4 model = translationMatrix * rotationMatrix * scaleMatrix;

    vertex_position = pos;
    old_vertex_position = view * model * vec4(oldPosition,1);
    gl_Position  = view * model * vec4(pos.x, pos.y, pos.z, 1.0);
}
