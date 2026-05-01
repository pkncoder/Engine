#version 330 core

in vec3 vNormal;
in vec3 vWorldPos;

out vec4 FragColor;

uniform vec3 u_ViewPos;   // The Camera's position in world space
// uniform vec3 u_LightPos;  // The Light's position in world space

void main() {

    vec3 u_LightPos = u_ViewPos;

    // Basic Properties
    vec3 lightColor = vec3(1.0);
    vec3 matColor = vec3(0.4, 0.8, 0.5);
    vec3 normal = normalize(vNormal);

    // 1. Ambient
    vec3 ambient = 0.15 * lightColor * matColor;

    // 2. Diffuse
    vec3 lightDir = normalize(u_LightPos - vWorldPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * matColor;

    // 3. Specular (Blinn-Phong)
    // Blinn-Phong uses the "Halfway Vector" between light and view
    vec3 viewDir = normalize(u_ViewPos - vWorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 
    
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0); // 32 is shininess
    vec3 specular = spec * lightColor * 0.5; // 0.5 is specular strength

    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(vec3(vNormal), 1.0);
    // FragColor = vec4(result, 1.0);

}
