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
in vec3 v_normal;
in vec3 v_worldPos;

out vec4 FragColor;

uniform vec3 u_viewPos;   // The Camera's position in world space

uniform vec3 u_albedo;
uniform vec3 u_emmissive;
uniform float u_roughness;
uniform float u_metallic;
// END INCLUDE: ../uniforms.glsl

// BEGIN INCLUDE: ../models/blinn_phong.glsl
// BEGIN INCLUDE: ../../include/modelBases/blinn_phong_base.glsl
// TODO: Double-check the implementation of this
vec3 blinnPhongMath(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {

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
    float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    float specularPower = abs(objectMaterial.roughness - 1.0); // TODO: Fix
    vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}
// END INCLUDE: ../../include/modelBases/blinn_phong_base.glsl

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {
    // Pass in the math
    return blinnPhongMath(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}
// END INCLUDE: ../models/blinn_phong.glsl

void main() {

    if (length(u_emmissive) > 0.01) {
        FragColor = vec4(u_emmissive, 1.0);
        return;
    }

    vec3 color = blinnPhong(u_viewPos, v_worldPos, v_normal, Material(u_albedo, u_emmissive, u_roughness, u_metallic), u_viewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
