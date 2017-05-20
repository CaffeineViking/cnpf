#version 410 core

layout (location = 0) in vec3 position;
uniform mat4 view;

out vec3 vertex_position;

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
}
