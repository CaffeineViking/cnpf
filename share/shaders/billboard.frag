#version 410 core

in vec3 final_vertex_position;
out vec4 color;

in vec2 texCoords;
uniform sampler2D diffuse;

void main()
{
    vec2 uv = texCoords.xy;
    color = texture(diffuse, uv);
}
