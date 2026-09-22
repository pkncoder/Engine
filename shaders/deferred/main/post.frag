#version 330 core

#include "../../include/utils/srgb.glsl"
#include "../../include/utils/toneMapping.glsl"

out vec4 FragColor;
uniform vec2 uResolution;

uniform sampler2D u_GBuffer0; // Position (RGB)
uniform sampler2D uColorRender; // Final output

uniform float uExposure;

uniform int uSRGB;
uniform int uToneMap;

uniform int uFog;
uniform vec3 uFogColor;
uniform float uFogDensity;

uniform int uVignette;
uniform float uVignetteRadius;
uniform float uVignetteSoftness;

// Camera Data
layout(std140) uniform CameraUBO {
    vec4 uCameraPos; 
    mat4 uViewProjection;
    mat4 uInverseView;
};

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;
    vec3 worldPos = texture(u_GBuffer0, uv).xyz;

    // Main color
    vec3 color = texture(uColorRender, uv).rgb;

    // Exposure
    color *= uExposure;

    if (uToneMap == 1) {
        // Tone mapping
        color = ACESFilm(color);
    }

    if (uSRGB == 1) {
      color = linearToSRGB(color);
    }

    if (uFog == 1) {
        // Fog
        float fogDist = length(uCameraPos.xyz - worldPos);
        float fogFactor = exp(-pow(fogDist * uFogDensity, 2.0));
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        color = mix(uFogColor, color, fogFactor);
    }

    if (uVignette == 1) {
        // Vigette
        vec2 centerCoord = uv * 2.0 - 1.0;
        float centerDist = length(centerCoord);
        float vignette = smoothstep(uVignetteRadius, uVignetteRadius - uVignetteSoftness, centerDist);
        color *= vignette;
    }

    FragColor = vec4(color, 1.0);
    return;
}
