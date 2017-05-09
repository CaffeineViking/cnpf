int wang_hash(int seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

void __kernel timers(
    __global float* timers,
    __global float* positions,
    const float frameDelta,
    const float threshold,
    __global float* positionsBuffer,
    const unsigned particleCount,
    const unsigned positionsBufferSize
    )
{
    int id = get_global_id(0);
    if(timers[id] >= threshold){
      timers[id] = 0.0f;
      float x = (((wang_hash(id) % 1000) - 500.0f)/500.0f) * 8.0f;
      float z = (((wang_hash(id+id) % 1000) - 500.0f)/500.0f) * 8.0f;
      positions[(id * 3) + 0] = x;
      positions[(id * 3) + 1] = -16;
      positions[(id * 3) + 2] = z;

      for(int i = 0; i < positionsBufferSize; i++){
	  positionsBuffer[i*particleCount*3 + 3*id+0] = x;
	  positionsBuffer[i*particleCount*3 + 3*id+1] = -16;
	  positionsBuffer[i*particleCount*3 + 3*id+2] = z;
      }
    }
    timers[id] += (fabs(frameDelta)) + (wang_hash(id) % 5) * 0.0001f;
}
