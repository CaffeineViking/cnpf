#version 410 core

in vec3 final_vertex_position;
out vec4 color;

in vec2 texCoords;
uniform sampler2D diffuse;

void main()
{
  vec2 uv = texCoords.xy;
  //uv.y *= -1.0;
  vec3 t = texture(diffuse, uv).rgb;
  // color based on uv and position
  color =  vec4(t, 1.0) * vec4(abs(normalize(final_vertex_position)),0.6);
} 
