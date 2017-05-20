#version 410 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform float size;
uniform mat4 projection;

in vec3 vertex_position[];
out vec3 final_vertex_position;
out vec2 texCoords;

void main()
{
    final_vertex_position = vertex_position[0];
    vec4 P = gl_in[0].gl_Position;
    
    // a: left-bottom
    vec2 va = P.xy + vec2(-0.5, -0.5) * size;
    gl_Position = projection * vec4(va, P.zw);
    texCoords = vec2(1.0, 1.0);
    EmitVertex();

    // b: left-top
    vec2 vb = P.xy + vec2(-0.5, 0.5) * size;
    gl_Position = projection * vec4(vb, P.zw);
    texCoords = vec2(1.0, 0.0);
    EmitVertex();

    // d: right-bottom
    vec2 vd = P.xy + vec2(0.5, -0.5) * size;
    gl_Position = projection * vec4(vd, P.zw);
    texCoords = vec2(0.0, 1.0);
    EmitVertex();

    // c: right-top
    vec2 vc = P.xy + vec2(0.5, 0.5) * size;
    gl_Position = projection * vec4(vc, P.zw);
    texCoords = vec2(0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
