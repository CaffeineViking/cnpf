
inline float smooth_step(float r)
{
   if(r<0.0f) return 0.0f;
   else if(r>1.0f) return 1.0f;
   // You notice an academic paper when the polynomials are factorized
   return r*r*r*(10.0f+r*(-15.0f+r*6.0f));
}



inline float ramp(float r)
{ return smooth_step((r+1)/2)*2-1; }

inline float smooth_step_p(float r, float r_lower, float r_upper, float value_lower, float value_upper)
{ return value_lower + smooth_step((r-r_lower)/(r_upper-r_lower)) * (value_upper-value_lower); }

float distance_and_normal(float3 p, float3 sphere, float radius, float3* normal){
	float phi = p.y;
	float3 u = p - sphere;
	float d = length(u);
	if(d - radius < phi){
		phi = d - radius;
		(*normal) = u/(d+((float)1e-10));
	} else {
		(*normal) = (float3)(0.0f,1.0f,0.0f);
	}
	return phi;
}

float3 match_boundrary(float3 psi, float d, float lengthscale, float3 normal){
	float alpha = fabs(ramp(d));
	return alpha * psi +(( 1.0f - alpha) * dot(normal, psi)) * normal;
}

float3 potential(float3 p, float3 np, read_only image3d_t t){
	const float3 	sphere_centre 	= (float3)(0.0f,0.0f,0.0f);
	const float 	sphere_radius 	= 8.0f;
	const float3 	field_direction = (float3)(0.0f,1.0f,0.0f);
	const float 	field_magnitude = 1.0f;
	const float 	noise_ratio 	= 0.0f;
	const sampler_t smp 			= CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT |CLK_FILTER_LINEAR;
	float4 noise = (read_imagef(t, smp, (float4)(np.x, np.y, np.z ,0.0f)) - 0.5f) * 2.0f;

    float3 psi = (float3)(0.0f,0.0f,0.0f);

    // Add Noise
    float3 psi_i = (float3)(noise.x, noise.y, noise.z);
    psi = psi_i * noise_ratio;

    float3 parallel = dot(field_direction, p);
    float3 ortho = p - parallel;
    float3 directional = cross(ortho, field_direction);

    psi += (1.0f - noise_ratio) * directional;

    float d = length(p - sphere_centre);

    float alpha = fabs(smoothstep(sphere_radius, sphere_radius + 1.0f, d));
    float3 n = normalize(p);
    return (alpha) * psi + (1.0f - alpha) * n * dot(n, psi);
}

float3 curl(float3 p, float3 np, read_only image3d_t t){
	const float eps = 0.0001f;
	const float3 epsX = (float3)(eps,0.0f,0.0f);
	const float3 epsY = (float3)(0.0f,eps,0.0f);
	const float3 epsZ = (float3)(0.0f,0.0f,eps);

	float3 curl = (float3)(0.0f,0.0f,0.0f);

      curl.x =( (potential(p + epsY,np + epsY,t).z - potential(p - epsY,np - epsY,t).z)
             -(	 potential(p + epsZ,np + epsZ,t).y - potential(p - epsZ,np - epsZ,t).y)) / (2*eps);
      curl.y =( (potential(p + epsZ,np + epsZ,t).x - potential(p - epsZ,np - epsZ,t).x)
             -(	 potential(p + epsX,np + epsX,t).z - potential(p - epsX,np - epsX,t).z)) / (2*eps);
      curl.z =( (potential(p + epsX,np + epsX,t).y - potential(p - epsX,np - epsX,t).y)
             -(	 potential(p + epsY,np + epsY,t).x - potential(p - epsY,np - epsY,t).x)) / (2*eps);
      return curl;
}

void __kernel particles(	__global float* positions,
							__global float* velocities,
							read_only image3d_t texture,
							const unsigned width,
							const unsigned height,
							const unsigned depth,
							const float frameDelta)
{
    int id = get_global_id(0);
    float x = positions[3*id+0];
    float y = positions[3*id+1];
    float z = positions[3*id+2];
    float3 position = (float3)(x,y,z);
    float3 noise_p = (float3)((x + width / 2.0f) / width,(y + height / 2.0f) / height,(z + depth / 2.0f) / depth);
  	float3 psi = curl(position,noise_p, texture);


   	positions[3*id+0] += psi.x  * frameDelta;//(values.x - 0.5f) * 2.0f * frameDelta  * velocities[3*id+0];
	positions[3*id+1] += psi.y  * frameDelta;//(values.y - 0.5f) * 2.0f * frameDelta  * velocities[3*id+1];
    positions[3*id+2] += psi.z  * frameDelta;//(values.z - 0.5f) * 2.0f * frameDelta  * velocities[3*id+2];
}