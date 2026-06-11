struct Ray {
    vec3 origin;
    vec3 direction;
    vec3 invDirection;
};

struct HitInfo {
    bool hit;
    float dist;

    vec3 hitPos;
    vec3 normal;

    int objectIndex;
};

struct Material {
    vec3 albedo;
    vec3 emmisive;

    float roughness;
    float metallic;
};
