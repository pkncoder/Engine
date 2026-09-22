#version 330 core

// BEGIN INCLUDE: ../../include/utils/srgb.glsl
vec3 vecLessThan(vec3 f, float value) {
    return vec3(
        (f.x < value) ? 1.0f : 0.0f,
        (f.y < value) ? 1.0f : 0.0f,
        (f.z < value) ? 1.0f : 0.0f
    );
}

vec3 linearToSRGB(vec3 rgb) {
    rgb = clamp(rgb, 0.0f, 1.0f);
    return mix(
        pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
        rgb * 12.92f,
        vecLessThan(rgb, 0.0031308f)
    );
}

vec3 SRGBToLinear(vec3 rgb) {
    rgb = clamp(rgb, 0.0f, 1.0f);
    return mix(
        pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
        rgb / 12.92f,
        vecLessThan(rgb, 0.04045f)
    );
}
// END INCLUDE: ../../include/utils/srgb.glsl
// BEGIN INCLUDE: ../../include/utils/toneMapping.glsl
vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}
// END INCLUDE: ../../include/utils/toneMapping.glsl

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
