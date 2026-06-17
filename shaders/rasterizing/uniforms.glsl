in vec3 v_normal;
in vec3 v_worldPos;
in vec2 v_TexCoords;

out vec4 FragColor;

uniform vec3 u_viewPos;   // The Camera's position in world space

uniform vec3 u_albedo;
uniform sampler2D u_albedoMap;
uniform int u_hasAlbedoMap;

uniform vec3 u_emmissive;
uniform float u_roughness;
uniform float u_metallic;
