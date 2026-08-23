#version 410 core
layout (location = 0) in vec3 aPos;

out vec4 FragPos;

uniform mat4 uModel;

void main() {
    // Just pass the world-space position to the Geometry Shader
    FragPos = uModel * vec4(aPos, 1.0);
    gl_Position = FragPos;
}
