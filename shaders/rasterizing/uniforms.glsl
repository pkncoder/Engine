in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform vec3 uViewPos;   // The Camera's position in world space

uniform vec3 uAlbedo;
uniform sampler2D uAlbedoMap;
uniform int uHasAlbedoMap;

uniform vec3 uEmmissive;
uniform float uRoughness;
uniform float uMetallic;
