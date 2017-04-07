int getIndex(int x, int y,int z, int width, int height){
    return x + width*y + width*height*z;
}
void kernel particles(global float* positions,
							global float* velocities,
							const unsigned width,
							const unsigned height,
							const unsigned depth,
							global float* noise, 
							const float time)
{
    int id = get_global_id(0);

    int x = ((int)positions[3*id+0] + width/2.0f);
    int y = ((int)positions[3*id+1] + height/2.0f);
    int z = ((int)positions[3*id+2] + depth/2.0f);
    int index = getIndex(x,y,z,width,height);
   	positions[3*id+0] += noise[(index*3)+0];//distanceX * cos(((float)id + time)) / ((float)id*0.005f ); //positions[3*id+0] + velocities[3*id+0] ;
	positions[3*id+1] += noise[(index*3)+1]; //velocities[3*id+1];//min(id * 0.001f, 6.0f);
    positions[3*id+2] += noise[(index*3)+2];//distanceZ * sin(((float)id+ time)) / ((float)id*0.005f);//positions[3*id+2] + velocities[3*id+2] ;
}