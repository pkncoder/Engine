#version 330 core

in vec3 v_normal;
in vec3 v_worldPos;

out vec4 FragColor;

uniform vec3 u_viewPos;   // The Camera's position in world space

uniform vec3 u_albedo;
uniform vec3 u_emmissive;
uniform float u_roughness;
uniform float u_metallic;

// BEGIN INCLUDE: ../include/sharedStructures.glsl
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
// END INCLUDE: ../include/sharedStructures.glsl
// BEGIN INCLUDE: ../include/models/blinn_phong.glsl
// TODO: Abstract the math and have the math be done in a seperate function
vec3 blinnPhong(const in Ray ray, const in HitInfo hit, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {

    // Basic Properties
    const vec3 lightColor = lightMaterial.emmisive;
    const vec3 objectColor = objectMaterial.albedo;
    const vec3 normal = hit.normal;

    // Ambient
    const vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    const vec3 lightDir = normalize(lightPos - hit.hitPos);
    const float nDotL = max(dot(normal, lightDir), 0.0);
    const vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    const vec3 viewDir = normalize(lightPos - hit.hitPos);
    const vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    const float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    const float specularPower = abs(objectMaterial.roughness - 1.0); // TODO: Fix
    const vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    const vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {

    // Basic Properties
    const vec3 lightColor = lightMaterial.emmisive;
    const vec3 objectColor = objectMaterial.albedo;

    // Ambient
    const vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    const vec3 lightDir = normalize(lightPos - worldPos);
    const float nDotL = max(dot(normal, lightDir), 0.0);
    const vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    const vec3 viewDir = normalize(viewPos - worldPos);
    const vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    const float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    const float specularPower = abs(objectMaterial.roughness - 1.0); // TODO: Fix
    const vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    const vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}

// END INCLUDE: ../include/models/blinn_phong.glsl

void main() {

    vec3 color = blinnPhong(u_viewPos, v_worldPos, v_normal, Material(u_albedo, u_emmissive, u_roughness, u_metallic), u_viewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
