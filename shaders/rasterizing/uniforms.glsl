
// Values from the vertex shader
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

// Final color output
out vec4 FragColor;

// Texture material values
uniform sampler2D uAlbedoMap;
uniform sampler2D uEmissiveMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetallicMap;
uniform sampler2D uAlphaMap;

// Normal map + uniform for bump map detection
uniform sampler2D uNormalMap;
uniform sampler2D uBumpMap;

uniform samplerCube uShadowCubeMap;

