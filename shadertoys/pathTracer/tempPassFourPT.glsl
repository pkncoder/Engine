#define MAX_TAA_FRAMES 32.0
#define TAA_MIN_MIX 0.05

// YUV-RGB conversion routines from Hyper3D
vec3 encodePalYuv(vec3 rgb)
{
    rgb = pow(rgb, vec3(2.0)); // gamma correction
    return vec3(
        dot(rgb, vec3(0.299, 0.587, 0.114)),
        dot(rgb, vec3(-0.14713, -0.28886, 0.436)),
        dot(rgb, vec3(0.615, -0.51499, -0.10001))
    );
}

vec3 decodePalYuv(vec3 yuv)
{
    vec3 rgb = vec3(
        dot(yuv, vec3(1., 0., 1.13983)),
        dot(yuv, vec3(1., -0.39465, -0.58060)),
        dot(yuv, vec3(1., 2.03211, 0.))
    );
    return pow(rgb, vec3(1.0 / 2.0)); // gamma correction
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Make sure the metadata pixel is saved
    if (ivec2(fragCoord) == ivec2(0, 0)) {
        fragColor = vec4(iMouse.xy / iResolution.xy, 0.0, 0.0);
        return;
    }
    
    #if (TAA == 1)

    // Get the uv
    vec2 uv  = fragCoord.xy / iResolution.xy;
    vec2 off = 1.0 / iResolution.xy;


    // Get the previous mouse position to get the previous camera statec
    vec2 prevMouseNorm = texelFetch(iChannel2, ivec2(0, 0), 0).xy;
    vec2 prevMouse     = prevMouseNorm * iResolution.xy;

    vec4  gBuffer  = texture(iChannel0, uv);
    float depth = gBuffer.z;

    // Reproject
    vec2 historyUV = uv; // fallback: no motion → straight screen-space lookup
    if (depth < SUPER_FAR) {
        
        // Get the worlposition and the reprojection
        vec3 worldPos = reconstructWorld(fragCoord, depth, iMouse.xy, iResolution.xy);
        vec2 reproj   = projectToScreen(worldPos, prevMouse, iResolution.xy);

        // Make sure that the reprojection is within the screen
        if (reproj.x > 1.5 * off.x && reproj.x < 1.0 &&
            reproj.y > 0.0          && reproj.y < 1.0) {
            historyUV = reproj; // Set the historyUV as the reprojection
        }
    }

    // Get this
    vec3 in0      = texture(iChannel1, uv).rgb;
    vec2 safeHist = clamp(uv, vec2(1.5) * off, vec2(1.0));
    vec4 lastData = texture(iChannel2, safeHist);

    vec3  antialiased  = lastData.rgb;
    float prevAccumN   = lastData.a;   // accumulation count stored in alpha

    // Get the accumulated number of frames and the new mix rate
    float accumN  = min(prevAccumN + 1.0, MAX_TAA_FRAMES);
    float mixRate = clamp(1.0 / accumN, TAA_MIN_MIX, 1.0);

    // Temporal blend
    antialiased = mix(antialiased * antialiased, in0 * in0, mixRate);
    antialiased = sqrt(max(antialiased, vec3(0.0)));

    // AABB Varience clip pixels
    vec3 in1 = texture(iChannel1, uv + vec2(+off.x, 0.0)).xyz;
    vec3 in2 = texture(iChannel1, uv + vec2(-off.x, 0.0)).xyz;
    vec3 in3 = texture(iChannel1, uv + vec2(0.0,  +off.y)).xyz;
    vec3 in4 = texture(iChannel1, uv + vec2(0.0,  -off.y)).xyz;
    vec3 in5 = texture(iChannel1, uv + vec2(+off.x, +off.y)).xyz;
    vec3 in6 = texture(iChannel1, uv + vec2(-off.x, +off.y)).xyz;
    vec3 in7 = texture(iChannel1, uv + vec2(+off.x, -off.y)).xyz;
    vec3 in8 = texture(iChannel1, uv + vec2(-off.x, -off.y)).xyz;

    // Encode every pixeel into YUV
    antialiased = encodePalYuv(antialiased);
    in0 = encodePalYuv(in0);
    in1 = encodePalYuv(in1);
    in2 = encodePalYuv(in2);
    in3 = encodePalYuv(in3);
    in4 = encodePalYuv(in4);
    in5 = encodePalYuv(in5);
    in6 = encodePalYuv(in6);
    in7 = encodePalYuv(in7);
    in8 = encodePalYuv(in8);

    // Get the AABB min and max
    vec3 minColor = min(min(min(in0, in1), min(in2, in3)), in4);
    vec3 maxColor = max(max(max(in0, in1), max(in2, in3)), in4);
    minColor = mix(minColor,
                   min(min(min(in5, in6), min(in7, in8)), minColor), 0.5);
    maxColor = mix(maxColor,
                   max(max(max(in5, in6), max(in7, in8)), maxColor), 0.5);

    // Save the value before the clip & apply the clip
    vec3 preclamping = antialiased;
    antialiased = clamp(antialiased, minColor, maxColor);

    // Get the AABB varience difference
    vec3  difference = antialiased - preclamping;
    float clampAmount = dot(difference, difference);

    // Calculate the new mixRate
    mixRate  = 1.0 / (1.0 / mixRate + 1.0);
    mixRate += clampAmount * 4.0;
    mixRate  = clamp(mixRate, 0.05, 0.5);

    // Decode YUV to get back to the normal colors
    antialiased = decodePalYuv(antialiased);

    // Return the final color
    fragColor = vec4(antialiased, accumN);
    
    #else
    fragColor = vec4(texture(iChannel1, fragCoord.xy / iResolution.xy).rgb, 0.0);
    #endif
}
