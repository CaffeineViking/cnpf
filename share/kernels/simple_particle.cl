void kernel simple_particle(global float* positions,
							global float* velocities,
							read_only image2d_t texture,
							const unsigned width,
							const unsigned height,
							const float time)
{
    int id = get_global_id(0);
    const sampler_t smp = CLK_NORMALIZED_COORDS_FALSE | //Natural coordinates
	CLK_ADDRESS_CLAMP | //Clamp to zeros
	CLK_FILTER_LINEAR; //Don't interpolate
	float4 values = read_imagef(texture, smp, (int2)(positions[3*id+0] + width/2.0f,positions[3*id+2] + height/2.0f));
	positions[3*id+1] = sqrt(pow(values.x,2)+pow(values.y,2)+pow(values.z,2)) * 100.0f;
	velocities[3*id+0] = values.x - 0.5f;
	//velocities[3*id+1] = values.y - 0.5f;
	velocities[3*id+2] = values.z - 0.5f;;
   	//ositions[3*id+0] += velocities[3*id+0];//distanceX * cos(((float)id + time)) / ((float)id*0.005f ); //positions[3*id+0] + velocities[3*id+0] ;
	//positions[3*id+1] += velocities[3*id+1];//min(id * 0.001f, 6.0f);
    //positions[3*id+2] += velocities[3*id+2];//distanceZ * sin(((float)id+ time)) / ((float)id*0.005f);//positions[3*id+2] + velocities[3*id+2] ;
}