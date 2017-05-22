int wang_hash(int seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return abs(seed);
}

float randf(int seed){
  return (((wang_hash(seed) % 100) - 50)/100.0f);
}

void __kernel timers(
    __global float* timers,
    __global float* positions,
    const float frameDelta,
    const float threshold,
    __global float* positionsBuffer,
    const unsigned particleCount,
    const unsigned positionsBufferSize,
    __global float* spawners,
    const unsigned spawnerCount
    )
{
    int id = get_global_id(0);
    if(timers[id] >= threshold){
      timers[id] = frameDelta *  randf(threshold *frameDelta + id + particleCount);
      int spawner = wang_hash(id * spawnerCount + threshold) % spawnerCount;

      float xo = spawners[spawner*6 + 3] / 2.0f;
      float yo = spawners[spawner*6 + 4] / 2.0f;
      float zo = spawners[spawner*6 + 5] / 2.0f;

      float x = spawners[spawner*6 + 0] + (xo * (randf(id * id)));
      float y = spawners[spawner*6 + 1] + (yo * (randf(threshold * id)));
      float z = spawners[spawner*6 + 2] + (zo * (randf(particleCount  * id)));

      positions[(id * 3) + 0] = x;
      positions[(id * 3) + 1] = y;
      positions[(id * 3) + 2] = z;

      for(int i = 0; i < positionsBufferSize; i++){
    	  positionsBuffer[i*particleCount*3 + 3*id+0] = x;
    	  positionsBuffer[i*particleCount*3 + 3*id+1] = y;
    	  positionsBuffer[i*particleCount*3 + 3*id+2] = z;
      }
    }
    timers[id] += frameDelta;
}


