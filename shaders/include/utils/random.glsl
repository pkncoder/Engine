uint seed; // Global seed

// Backup seed for when the blue noise texture isn't loaded
void setSeed(vec2 fragCoord, uint frameNum)
{
    seed = uint(fragCoord.x) * 1973u + uint(fragCoord.y) * 9277u + frameNum * 26699u | 1u;
}

// Hash function for random uint
uint wangHash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

// Random float one from [0-1]
float rnd1(inout uint seed){
    return float(wangHash(seed)) / float(-1u);
}

// Random normalized unit vector
vec3 rndUnit(inout uint seed) {
    float z = rnd1(seed) * 2.0f - 1.0f;
    float a = rnd1(seed) * 6.28318530718;
    float r = sqrt(1.0f - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}
