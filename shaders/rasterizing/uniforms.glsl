
// Values from the vertex shader
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

// Final color output
out vec4 FragColor;

// Render resolution
uniform vec2 uResolution;

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
uniform sampler2D uAlphaMap;

// Normal map + uniform for bump map detection
uniform sampler2D uNormalMap;
uniform sampler2D uBumpMap;
uniform int uIsBumpMap;

// Shadow mapping
uniform samplerCube uShadowCubeMap;
uniform float uShadowFarPlane;
uniform vec3 uLightPos;
