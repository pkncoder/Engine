void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

    fragColor.a = 1.0;
    
    // Get the frag color
    fragColor.rgb = texture(iChannel0, fragCoord / iResolution.xy).rgb;

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
