#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;

void main() {
    // Just pass the world-space position to the Geometry Shader
    gl_Position = uModel * vec4(aPos, 1.0);
}
