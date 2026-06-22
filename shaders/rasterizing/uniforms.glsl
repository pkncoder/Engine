
// Values from the vertex shader
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

// Final color output
out vec4 FragColor;

// Base material values
uniform vec3 uAlbedo;
uniform vec3 uEmmissive;
uniform float uRoughness;
uniform float uMetallic;

// Texture material values
uniform sampler2D uAlbedoMap;
uniform sampler2D uEmissiveMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetallicMap;

// Normal map + uniform for bump map detection
uniform sampler2D uNormalMap;
uniform int uIsBumpMap;
