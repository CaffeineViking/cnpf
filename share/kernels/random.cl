void random(global float* positions,
        int const dimx, int const dimy,
        uint const seed)
{
    int id = get_global_id(0);
    positions[3*id+0] = positions[3*id+0] ;
	positions[3*id+1] = 0.0f;
    positions[3*id+2] = positions[3*id+2] ;
}