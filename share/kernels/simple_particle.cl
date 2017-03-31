void kernel simple_particle(global float* positions, global float* velocities, const float time)
{
	float distanceX = 15.3f;
	float distanceZ =  15.3f;
    int id = get_global_id(0);

   	positions[3*id+0] = distanceX * cos(time+id) / ((float)id*0.01f); //positions[3*id+0] + velocities[3*id+0] ;
	positions[3*id+1] = id * 0.0001f;
    positions[3*id+2] = distanceZ * sin(time+id) / ((float)id*0.01f);//positions[3*id+2] + velocities[3*id+2] ;
}