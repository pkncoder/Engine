#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec3 vNormal;
out vec3 vWorldPos;
out vec2 vTexCoords;

void main() {

    // Set the world coords & tex coords
    vWorldPos = vec3(uModel * vec4(aPos, 1.0));
    vTexCoords = aTexCoords;

    // Set the normal
    vNormal = normalize(mat3(uModel) * aNormal);

    // Set the position
    gl_Position = uViewProjection * vec4(vWorldPos, 1.0);
}
