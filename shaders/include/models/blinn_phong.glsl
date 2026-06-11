// TODO: Abstract the math and have the math be done in a seperate function
vec3 blinnPhong(const in Ray ray, const in HitInfo hit, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {

    // Basic Properties
    vec3 lightColor = lightMaterial.emmisive;
    vec3 objectColor = objectMaterial.albedo;
    vec3 normal = hit.normal;

    // Ambient
    vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - hit.hitPos);
    float nDotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    vec3 viewDir = normalize(lightPos - hit.hitPos);
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

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {

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

