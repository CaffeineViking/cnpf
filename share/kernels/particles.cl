
inline float smooth_step(float r)
{
   if(r<0.0f) return 0.0f;
   else if(r>1.0f) return 1.0f;
   // You notice an academic paper when the polynomials are factorized
   return r*r*r*(10.0f+r*(-15.0f+r*6.0f));
}

inline float ramp(float r)
{ return smooth_step((r+1)/2)*2-1; }


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
	float alpha = ramp(fabs(d)/lengthscale);
	return alpha * psi +(( 1.0f - alpha) * dot(psi, normal)) * normal;
}

float3 potential(float3 p, float3 np, read_only image3d_t t){
	const float3 sphere_centre = (float3)(0.0f,0.0f,0.0f);
	const float sphere_radius = 16.0f;
	const float plume_height = 32.0f;
	const float ring_speed = 2.0f;
	const float ring_magnitude = 2.9f;
	const float ring_radius = 8.0f;
	const sampler_t smp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_MIRRORED_REPEAT |CLK_FILTER_LINEAR;
	float4 noise = (read_imagef(t, smp, (float4)(np.x, np.y, np.z ,0.0f)) - 0.5f) * 2.0f;

    float3 psi = (float3)(0.0f,0.0f,0.0f);
    float3 normal;
    float d = distance_and_normal(p,sphere_centre, sphere_radius, &normal);
    float height_factor = ramp(p.y/plume_height);

    float3 psi_i = (float3)(noise.x, noise.y, noise.z);
    psi_i = match_boundrary(psi_i,d,0.2f,normal);
    psi += height_factor * psi_i;

    float ring_y = ring_speed;
    while(ring_y > 0.0f) {    	
    	float ry = p.y - ring_y;    	
    	float rr = sqrt(p.x*p.x + p.z*p.z);
    	float rmag = ring_magnitude;
    	float3 rpsi = rmag * (float3)(p.z,0.0f,-p.x);
    	rpsi = match_boundrary(rpsi,d,ring_radius, normal);
    	psi += rpsi;
		ring_y -=ring_speed;
    }
    return psi;
}

float3 curl(float3 p, float3 np, read_only image3d_t t){
	const float eps = 0.0002f;
	const float3 epsX = (float3)(eps,0.0f,0.0f);
	const float3 epsY = (float3)(0.0f,eps,0.0f);
	const float3 epsZ = (float3)(0.0f,0.0f,eps);

	float3 curl = (float3)(0.0f,0.0f,0.0f);

      curl.x =( (potential(p + epsY,np + epsY,t).z - potential(p - epsY,np - epsY,t).z)
             -(	 potential(p + epsZ,np + epsZ,t).y - potential(p - epsZ,np - epsZ,t).y)) / (2*eps);
      curl.y =( (potential(p + epsZ,np + epsZ,t).x - potential(p - epsZ,np - epsZ,t).x)
             -(	 potential(p + epsX,np + epsX,t).z - potential(p - epsX,np - epsX,t).z)) / (2*eps);
      curl.z =( (potential(p + epsX,np + epsX,t).y - potential(p - epsX,np - epsX,t).y)
             -(	 potential(p + epsX,np + epsX,t).x - potential(p - epsY,np - epsY,t).x)) / (2*eps);
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


   	positions[3*id+0] += psi.x  * frameDelta * 2.0f;//(values.x - 0.5f) * 2.0f * frameDelta  * velocities[3*id+0];
	positions[3*id+1] += psi.y  * frameDelta * 2.0f;//(values.y - 0.5f) * 2.0f * frameDelta  * velocities[3*id+1];
    positions[3*id+2] += psi.z  * frameDelta * 2.0f;//(values.z - 0.5f) * 2.0f * frameDelta  * velocities[3*id+2];
}