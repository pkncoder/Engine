#version 330 core

layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aCol;

uniform mat4 u_ViewProjection;

out vec3 vCol;

void main() {
    vCol = vec3(1.0, 0.0, 1.0);
    gl_Position = u_ViewProjection * vec4(aPos, 1.0);
}
