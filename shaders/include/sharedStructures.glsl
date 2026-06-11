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

struct Material {
    vec3 albedo;
    vec3 emmisive;

    float roughness;
    float metallic;
};
