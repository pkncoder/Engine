/*

Credit + explanation will be linked soon, however I am also in the midst of porting this to an engine I'm working on.

Hello! This is my Path Tracer Engine demo. The Path Tracer utilizes the following utilities:
- GBuffer
- MIS (Multiple Importance Sampling) + NEE (Next Event Estimation)
- Denoiser (Edge-Avoiding À-Trous Wavelet Transform)
- TAA (Temporal Anti-Aliasing)

The code/implementation is based off of a few programs, notably 
@levraidantenoe46's "Realtime pathtracer denoiser" THIS HAS BEEN DELETED :(

Bits are also pulled from Demofox's "Casual Shadertoy Path Tracing" blog.

Mouse can be used as an orbit camera, no keys.

***Check common tab for tons of settings, and the code is pretty much fully commented.***

note: I AM STILL TUNING RIGHT NOW

*/

#define SPLITLINE 1

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

    fragColor.a = 1.0;
    
    // Get the final pass's color
    if (SPLITLINE == 0 || fragCoord.x/iResolution.x < 0.499) {
        fragColor.rgb = texture(iChannel0, fragCoord / iResolution.xy).rgb;
    } else if (fragCoord.x/iResolution.x > 0.501) {
        fragColor.rgb = texture(iChannel2, fragCoord / iResolution.xy).rgb;
    } else {
        fragColor = vec4(1.0);
        return;
    }

    // Multiply by the exposure
    #if (DO_EXPOSURE == 1)
    fragColor.rgb *= EXPOSURE;
    #endif

    // HDR -> SDR
    #if (SDR == 1)
    fragColor.rgb = ACESFilm(fragColor.rgb);
    #endif

    // Linear -> SRGB
    #if (SRGB == 1)
    fragColor.rgb = LinearToSRGB(fragColor.rgb);
    #endif
}
