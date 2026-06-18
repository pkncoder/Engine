#version 330 core

// BEGIN INCLUDE: ../../include/common.glsl
// Numbers
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define INV_PI 0.31830988618
#define INV_TWO_PI  0.15915494309
#define EPSILON 0.001
#define FAR 25.0
#define REALLY_FAR 50.0
#define SUPER_FAR 90.0

// Get the local basis matrix
// TODO: move
mat3 getBasis(vec3 n) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    return mat3(t, b, n);
}
// END INCLUDE: ../../include/common.glsl
// BEGIN INCLUDE: ../../include/sharedStructures.glsl
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
    uint materialIndex;
};

// TODO: Merge
struct Material {
    vec3 albedo;
    vec3 emmisive;

    float roughness;
    float metallic;
};

struct GPUMaterial {
    vec4 albedo;
    vec4 emissive;
    float roughness;
    float metallic;
};
// END INCLUDE: ../../include/sharedStructures.glsl

// BEGIN INCLUDE: ../uniforms.glsl
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform vec3 uViewPos;   // The Camera's position in world space

uniform vec3 uAlbedo;
uniform sampler2D uAlbedoMap;
uniform int uHasAlbedoMap;

uniform vec3 uEmmissive;
uniform float uRoughness;
uniform float uMetallic;
// END INCLUDE: ../uniforms.glsl

// BEGIN INCLUDE: ../models/blinnPhong.glsl
// BEGIN INCLUDE: ../../include/modelBases/blinnPhongBase.glsl
// TODO: Double-check the implementation of this
vec3 blinnPhongBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {

    // Basic Properties
    vec3 lightColor = lightMaterial.emmisive;
    vec3 objectColor = objectMaterial.albedo;

    // Ambient
    vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - worldPos);
    float nDotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 32.0); 
    float specularPower = abs(objectMaterial.roughness - 1.0);
    vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}
// END INCLUDE: ../../include/modelBases/blinnPhongBase.glsl

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {
    // Pass in the math
    return blinnPhongBase(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}
// END INCLUDE: ../models/blinnPhong.glsl

void main() {

    if (length(uEmmissive) > 0.01) {
        FragColor = vec4(uEmmissive, 1.0);
        return;
    }

    vec3 baseColor;
    if (uHasAlbedoMap == 1) {
        vec4 texColor = texture(uAlbedoMap, vTexCoords);
        baseColor = texColor.rgb * uAlbedo;
    } else {
        baseColor = uAlbedo;
    }

    vec3 color = blinnPhong(uViewPos, vWorldPos, vNormal, Material(baseColor, uEmmissive, uRoughness, uMetallic), uViewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
