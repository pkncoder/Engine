in vec3 v_normal;
in vec3 v_worldPos;

out vec4 FragColor;

uniform vec3 u_viewPos;   // The Camera's position in world space

uniform vec3 u_albedo;
uniform vec3 u_emmissive;
uniform float u_roughness;
uniform float u_metallic;
