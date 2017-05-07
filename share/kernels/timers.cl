int wang_hash(int seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float randf(int seed){
  return (((wang_hash(seed) % 1000) - 500.0f)/500.0f);
}

void __kernel timers(
    __global float* timers,
    __global float* positions,
    const float frameDelta,
    const float threshold
    )
{
    int id = get_global_id(0);
    if(timers[id] >= threshold){
      timers[id] = frameDelta *  randf(threshold *frameDelta * id);
      float x =  randf(id) * 8.0f;
      float z =  randf(id+id) * 8.0f;
      positions[(id * 3) + 0] = x;
      positions[(id * 3) + 1] = -16;
      positions[(id * 3) + 2] = z;
    }
    timers[id] += (fabs(frameDelta)) + randf(threshold *frameDelta * id) * 0.001f;
}