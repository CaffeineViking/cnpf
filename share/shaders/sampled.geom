#version 430 core
#define MAX_POSITIONS_BUFFERS 8

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 16) out;

uniform float size;
uniform mat4 projection;
uniform int segment_count;

in vec3 vertex_position[];
in vec4 old_vertex_positions[][MAX_POSITIONS_BUFFERS];
out vec3 final_vertex_position;
out vec2 texCoords;

void createQuad(vec4 V0, vec4 V1, float uvStart, float uvEnd){
    vec2 v;

    // bottom left
    v = V0.xy + vec2(-0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V0.zw);
    texCoords = vec2(1.0, uvStart);
    EmitVertex();

    // bottom right
    v = V0.xy + vec2(0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V0.zw);
    texCoords = vec2(0.0, uvStart);
    EmitVertex();

    // top left
    v = V1.xy + vec2(-0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V1.zw);
    texCoords = vec2(1.0, uvEnd);
    EmitVertex();

    EndPrimitive();

    // top left
    v = V1.xy + vec2(-0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V1.zw);
    texCoords = vec2(1.0, uvEnd);
    EmitVertex();
    
    // bottom right
    v = V0.xy + vec2(0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V0.zw);
    texCoords = vec2(0.0, uvStart);
    EmitVertex();

    // top right
    v = V1.xy + vec2(0.5, 0.0) * size;
    gl_Position = projection * vec4(v, V1.zw);
    texCoords = vec2(0.0, uvEnd);
    EmitVertex();
    
    EndPrimitive();
}

vec4 interpolatedPoint(float index){
    if (index >= MAX_POSITIONS_BUFFERS-1)
	return old_vertex_positions[0][MAX_POSITIONS_BUFFERS-1];
    else { 
	vec4 p1 = old_vertex_positions[0][int(floor(index))];
	vec4 p2 = old_vertex_positions[0][int(ceil(index))];
	float weight = index - floor(index);
	
	return vec4(p1.xyz + weight*(p2.xyz - p1.xyz), 1.0);
    }
}

void main()
{
    final_vertex_position = vertex_position[0];
    vec4 P = gl_in[0].gl_Position;
    vec4 V0, V1;
    float uv0, uv1;
    
    float stride = MAX_POSITIONS_BUFFERS / float(segment_count);
    V1 = P;
    uv1 = 0.0;
    for(int i = segment_count-1; i >= 0; i--){
	V0 = interpolatedPoint(i*stride);
	uv0 = 1.0 - (i*stride) / MAX_POSITIONS_BUFFERS;
	createQuad(V0, V1, uv0, uv1);
	V1 = V0;
	uv1 = uv0;
    }
}
