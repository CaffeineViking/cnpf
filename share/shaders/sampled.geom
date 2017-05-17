#version 430 core
#define MAX_POSITIONS_BUFFERS 8

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 12) out;

uniform float size;
uniform mat4 projection;

in vec3 vertex_position[];
in vec4 old_vertex_positions[][MAX_POSITIONS_BUFFERS];
out vec3 final_vertex_position;
out vec2 texCoords;

void createQuad(vec4 V0, vec4 V1, float uvStart, float uvEnd){
    vec2 v;

    // a: left 1
    v = V0.xy + vec2(-0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V0.zw);
    texCoords = vec2(1.0, uvStart);
    EmitVertex();

    // b: right 1
    v = V0.xy + vec2(0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V0.zw);
    texCoords = vec2(0.0, uvStart);
    EmitVertex();

    // d: left 2
    v = V1.xy + vec2(-0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V1.zw);
    texCoords = vec2(1.0, uvEnd);
    EmitVertex();

    EndPrimitive();

    
    // d: left 2
    v = V1.xy + vec2(-0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V1.zw);
    texCoords = vec2(1.0, uvEnd);
    EmitVertex();
    
    // b: right 1
    v = V0.xy + vec2(0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V0.zw);
    texCoords = vec2(0.0, uvStart);
    EmitVertex();

    // right 2
    v = V1.xy + vec2(0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V1.zw);
    texCoords = vec2(0.0, uvEnd);
    EmitVertex();
    
    EndPrimitive();
}


void main()
{
    final_vertex_position = vertex_position[0];
    vec4 P = gl_in[0].gl_Position;
    vec4 V0 = old_vertex_positions[0][0];
    vec4 V1 = old_vertex_positions[0][3];
    
    createQuad(V0, V1, 1.0, 0.5);
    createQuad(V1, P, 0.5, 0.0);
    
    // // left 2
    // v = V1.xy + vec2(-0.5, 0.0) * size;
    // gl_Position = projection * vec4(v, V1.zw);
    // texCoords = vec2(1.0, 0.5);
    // EmitVertex();

    // // left 3
    // v = P.xy + vec2(-0.5, 0.0) * size;
    // gl_Position = projection * vec4(v, P.zw);
    // texCoords = vec2(1.0, 0.0);
    // EmitVertex();

    // // right 2
    // v = V1.xy + vec2(0.5, 0.0) * size;
    // gl_Position = projection * vec4(v, V1.zw);
    // texCoords = vec2(0.0, 0.5);
    // EmitVertex();

    // // right 3
    // v = P.xy + vec2(0.5, 0.0) * size;
    // gl_Position = projection * vec4(v, P.zw);
    // texCoords = vec2(0.0, 0.0);
    // EmitVertex();

    // EndPrimitive();
}
