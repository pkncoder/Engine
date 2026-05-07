vec3 blinnPhong(Ray ray, HitInfo hit) {

    // Light position is just the camera at the moment
    const vec3 lightPos = u_cameraPos;

    // Basic Properties
    const vec3 lightColor = vec3(1.0);
    const vec3 matColor = vec3(0.4, 0.8, 0.5);
    const vec3 normal = hit.normal;

    // Ambient
    const vec3 ambient = 0.15 * lightColor * matColor;

    // Diffuse
    const vec3 lightDir = normalize(lightPos - hit.hitPos);
    const float diffuseStrength = max(dot(normal, lightDir), 0.0);
    const vec3 diffuse = lightColor * matColor * diffuseStrength;

    // Get the view & halfway vectors
    const vec3 viewDir = normalize(u_cameraPos - hit.hitPos);
    const vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    const float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    const float specularPower = 0.5;
    const vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    const vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}

