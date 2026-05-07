struct Ray {
    vec3 origin;
    vec3 direction;
};

struct HitInfo {
    bool hit;
    float dist;
    vec3 hitPos;
    vec3 normal;
};
