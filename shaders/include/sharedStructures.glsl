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

    uint objectIndex;
    uint materialIndex;
};

struct Material {
    vec4 albedo;
    vec4 emissive;
    float roughness;
    float metallic;
};
