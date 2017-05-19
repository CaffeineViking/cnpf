
//Used during execution, it beats the parameter bucket brigade previously used
typedef struct Params {
  float width;
  float height;
  float depth;
  float field_magnitude;
  float noise_ratio;
  float noise_width;
  float noise_height;
  float noise_depth;
  float boundrary_width;
  float fieldX;
  float fieldY;
  float fieldZ;
} Params;

float smooth(float p1, float p2, float d){
   float r = (d - p1)/(p2-p1);
   if(r<0.0f) return 0.0f;
   else if(r>1.0f) return 1.0f;
   return r*r*r*(10.0f+r*(-15.0f+r*6.0f));
}

float get_closest_sphere(float3 p, const int nsph, __global float* sph, float3* centre){
  float best = INFINITY;
  float best_radius = 0.0f;
  (*centre) = (float3)(0.0f,0.0f,0.0f);
  for(int i = 0; i < nsph; i++){
    float x = sph[i*4 + 0];
    float y = sph[i*4 + 1];
    float z = sph[i*4 + 2];
    float r = sph[i*4 + 3];

    float d = distance(p,(float3)(x,y,z));
    if(d < best){
      best = d;
      (*centre) = (float3)(x,y,z);
      best_radius = r;
    }
  }
  return best_radius;
}

float getAlpha(float sphere_radius, float influence_radius, float distance){
  return fabs(((float)smooth(sphere_radius, sphere_radius + influence_radius, distance)));
}

float3 potential(float3 p, float3 np, read_only image3d_t t,const int nsph, __global float* sph, const Params params){

  const float3  sphere_centre   = (float3)(0.0f,0.0f,0.0f);
  const float   sphere_radius   = get_closest_sphere(p,nsph, sph, &sphere_centre);
  const float3  field_direction = (float3)(params.fieldX,params.fieldY, params.fieldZ);
  const sampler_t smp       = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT |CLK_FILTER_LINEAR;
  float4 noise = (read_imagef(t, smp, (float4)(np.x, np.y, np.z ,0.0f)) - 0.5f) * 2.0f;

    float3 psi = (float3)(0.0f,0.0f,0.0f);

    // Add Noise
    float3 psi_i = (float3)(noise.x, noise.y, noise.z);
    psi = psi_i * params.noise_ratio;

    float3 parallel = dot(field_direction, p) * field_direction;
    float3 ortho = p - parallel;
    float3 directional = cross(ortho, field_direction);

    psi += (1.0f - params.noise_ratio) * directional * params.field_magnitude;

    float d = length(p - sphere_centre);

    float alpha = getAlpha(sphere_radius, params.boundrary_width, d);
    float3 n = normalize(p);
    return (alpha) * psi + (1.0f - alpha) * n * dot(n, psi);
}

float3 curl(float3 p, float3 np, read_only image3d_t t,const int nsph,  __global float* sph, const Params parameters){
  const float eps = 0.0001f;
  const float3 epsX = (float3)(eps,0.0f,0.0f);
  const float3 epsY = (float3)(0.0f,eps,0.0f);
  const float3 epsZ = (float3)(0.0f,0.0f,eps);

  float3 curl = (float3)(0.0f,0.0f,0.0f);

      curl.x =( (potential(p + epsY,np + epsY,t, nsph, sph, parameters).z - potential(p - epsY,np - epsY,t, nsph, sph, parameters).z)
             -(  potential(p + epsZ,np + epsZ,t, nsph, sph, parameters).y - potential(p - epsZ,np - epsZ,t, nsph, sph, parameters).y)) / (2*eps);
      curl.y =( (potential(p + epsZ,np + epsZ,t, nsph, sph, parameters).x - potential(p - epsZ,np - epsZ,t, nsph, sph, parameters).x)
             -(  potential(p + epsX,np + epsX,t, nsph, sph, parameters).z - potential(p - epsX,np - epsX,t, nsph, sph, parameters).z)) / (2*eps);
      curl.z =( (potential(p + epsX,np + epsX,t, nsph, sph, parameters).y - potential(p - epsX,np - epsX,t, nsph, sph, parameters).y)
             -(  potential(p + epsY,np + epsY,t, nsph, sph, parameters).x - potential(p - epsY,np - epsY,t, nsph, sph, parameters).x)) / (2*eps);
      return normalize(curl);
}

void __kernel particles(
  __global float* positions,
  __global float* spheres,
  const int nrSpeheres,
  read_only image3d_t texture,
  const Params parameters,
  const float frameDelta,
  __global float* positionsBuffer,
  const unsigned particleCount,
  const unsigned positionsBufferHead)
{
    int id = get_global_id(0);
    float x = positions[3*id+0];
    float y = positions[3*id+1];
    float z = positions[3*id+2];
    float3 position = (float3)(x,y,z);
    float3 noise_p = (float3)(x / parameters.noise_width,y / parameters.noise_height , z / parameters.noise_depth);
   float3 psi = curl(position,noise_p, texture, nrSpeheres, spheres, parameters);

    positions[3*id+0] += psi.x * 4.0f * frameDelta;//(values.x - 0.5f) * 2.0f * frameDelta  * velocities[3*id+0];
    positions[3*id+1] += psi.y * 4.0f * frameDelta;//(values.y - 0.5f) * 2.0f * frameDelta  * velocities[3*id+1];
    positions[3*id+2] += psi.z * 4.0f * frameDelta;//(values.z - 0.5f) * 2.0f * frameDelta  * velocities[3*id+2];

    positionsBuffer[positionsBufferHead*particleCount*3 + 3*id+0] = positions[3*id+0];
    positionsBuffer[positionsBufferHead*particleCount*3 + 3*id+1] = positions[3*id+1];
    positionsBuffer[positionsBufferHead*particleCount*3 + 3*id+2] = positions[3*id+2];
}

__kernel void exportCurl(
  __write_only image2d_t image,
  __global float* spheres, const int nrSpeheres,read_only image3d_t texture, const Params parameters, const float scaleFactor)
{
  const int x = get_global_id(0);
  const int y = 0;
  const int z = get_global_id(1) ;
  const int2 coords = (int2)(x,z);
  const float3 hd = (float3)(parameters.width, parameters.height, parameters.depth) * 0.5f;
  float4 color;

   float3 position = ((float3)(x,y,z) - hd) * scaleFactor;
   float3 noise_p = ((float3)(x,y,z) - hd) * scaleFactor;
   float3 psi = curl(position,noise_p, texture, nrSpeheres, spheres, parameters);
   color.x = psi.x;
   color.y = psi.y;
   color.z = psi.z;
   color += 1.0f;
   color *= 0.5f;
   color.w = 1.0f;
  write_imagef(image, coords, color);
}

__kernel void exportDistance(
  __write_only image2d_t image,
  __global float* spheres, const int nrSpeheres,read_only image3d_t texture, const Params parameters, const float scaleFactor)
{
  const int x = get_global_id(0);
  const int y = 0;
  const int z = get_global_id(1) ;
  const int2 coords = (int2)(x,z);
  const float3 hd = (float3)(parameters.width, parameters.height, parameters.depth) * 0.5f;
  float4 color;

   float3 position = ((float3)(x,y,z) - hd) * scaleFactor;
   const float3  sphere_centre   = (float3)(0.0f,0.0f,0.0f);
   const float   sphere_radius   = get_closest_sphere(position,nrSpeheres, spheres, &sphere_centre);
  float d = length(position - sphere_centre);
  float alpha = getAlpha(sphere_radius, 4.0f, d);
   color.x = alpha;
   color.y = alpha;
   color.z = alpha;
   color += 1.0f;
   color *= 0.5f;
   color.w = 1.0f;
  write_imagef(image, coords, color);
}