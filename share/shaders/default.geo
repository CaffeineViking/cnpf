#version 150

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;    
   
// GLSL Hacker automatic uniforms:
uniform mat4 MVP;



   
void main (void)
{
  float size = 1.0;

  vec3 right = vec3(MV[0][0], 
                    MV[1][0], 
                    MV[2][0]);

  vec3 up = vec3(MV[0][1], 
                 MV[1][1], 
                 MV[2][1]);
  
  vec3 P = gl_in[0].gl_Position.xyz;
 
  vec3 va = P - (right + up) * size;
  gl_Position = MVP * vec4(va, 1.0);
  EmitVertex();  
  
  vec3 vb = P - (right - up) * size;
  gl_Position = MVP * vec4(vb, 1.0);
  EmitVertex();  

  vec3 vd = P + (right - up) * size;
  gl_Position = MVP * vec4(vd, 1.0);
  EmitVertex();  

  vec3 vc = P + (right + up) * size;
  gl_Position = MVP * vec4(vc, 1.0);
  EmitVertex();  
  
  EndPrimitive();  
}   