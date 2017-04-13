void kernel particles(		global float* positions,
							global float* velocities,
							read_only image3d_t texture,
							const unsigned width,
							const unsigned height,
							const unsigned depth,
							const float frameDelta)
{
    int id = get_global_id(0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE  |CLK_FILTER_LINEAR;
    float x = positions[3*id+0] + width/2.0f ;
    float y = positions[3*id+1] + height/2.0f;
    float z = positions[3*id+2] + depth/2.0f ;
	float4 values = read_imagef(texture, smp, (float4)(x,y,z,0.0f));
   	positions[3*id+0] += (values.x - 0.5f) * frameDelta * 2 * velocities[3*id+0];
	positions[3*id+1] += (values.y - 0.5f) * frameDelta * 2 * velocities[3*id+1];
    positions[3*id+2] += (values.z - 0.5f) * frameDelta * 2 * velocities[3*id+2];
}