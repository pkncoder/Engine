#version 330 core

in vec3 vCol;
in vec3 attrPos;
in vec3 vNormal;
out vec4 FragColor;

uniform vec3 lookingDir;

void main() {


    // vec3 lightPos = vec3(3.0, 3.0, 2.0);
    vec3 lightPos = lookingDir;

    vec3 lightColor = vec3(1.0);

    vec3 lightDir = normalize(lightPos - attrPos);
    vec3 matColor = vec3(0.4, 0.8, 0.5);

    vec3 ambient = 0.15 * lightColor * matColor;

    vec3 diffuse = max(dot(vNormal, lightDir), 0.0) * lightColor * matColor;

    vec3 reflectedDir = reflect(-lightDir, vNormal);
    vec3 specular = pow(max(dot(normalize(lookingDir - attrPos), reflectedDir), 0.0), 16) * lightColor * 1.0;

    vec3 col = (ambient + diffuse + specular);


    FragColor = vec4(col, 1.0);
}
