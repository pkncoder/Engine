#define DENOISE_RANGE vec2(1.0, 4.0)
#define MAX_HISTORY 16.0

void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    #if (DENOISE == 1)

    /* SETUP */

    // Store iMouse for the next frame's camera state reconstruction
    if (ivec2(fragCoord) == ivec2(0, 0)) {
        fragColor = vec4(iMouse.xy / iResolution.xy, 0.0, 0.0);
        return;
    }

    setSeed(fragCoord, uint(iFrame), iChannel3);

    // Get the texture size and center coord for this pass
    vec2 uv = fragCoord/iResolution.xy;

    vec2 centerCoord = fragCoord;
    vec3 centerColor = texture(iChannel1, uv).rgb;


    /* G-BUFFER */

    // Get the center G-Buffer
    vec4 gBuffer = texture(iChannel0, fragCoord/iResolution.xy);

    float centerDepth = gBuffer.z;
    if (centerDepth >= SUPER_FAR) {
        fragColor = vec4(centerColor, 1.0);
        return;
    }

    // G-Buffer attributes
    vec3 centerNormal = unpackNormal(gBuffer.xy);


    /* --- TEMPORAL REPROJECTION --- */

    // Get the previous mouse position to get the previous camera state
    vec2 prevMouseNorm = texelFetch(iChannel2, ivec2(0, 0), 0).xy;
    vec2 prevMouse     = prevMouseNorm * iResolution.xy;

    // Reproject
    vec3 worldPos = reconstructWorld(fragCoord, centerDepth, iMouse.xy, iResolution.xy);
    vec2 reproj   = projectToScreen(worldPos, prevMouse, iResolution.xy);

    vec2 reprojectedUV = uv; // fallback: no motion → straight screen-space lookup
    bool validHistory = true;

    // Caclulate the width of a pixel and create a margin
    vec2 off = 1.0 / iResolution.xy;
    vec2 margin = 1.5 * off;

    // Check to see if the reprojection is within the screen space - the margin
    if (reproj.x > margin.x && reproj.x < (1.0 - margin.x) &&
        reproj.y > margin.y && reproj.y < (1.0 - margin.y)) {
        reprojectedUV = reproj; // Set the historyUV as the reprojection
    } else {
        validHistory = false;
    }

    // Set history to false during the first few frames to stop frame-0 from darkening the rest
    if (iFrame < 5) validHistory = false;

    vec2 motionVector = reprojectedUV - uv;
    vec2 reprojectedCoord = centerCoord + motionVector;

    vec4 reprojectionData = texture(iChannel2, reprojectedUV);

    vec3 historyColor = reprojectionData.rgb;
    float historyLength = validHistory ? (reprojectionData.a * MAX_HISTORY) : 0.0;


    /* MATERIAL FETCHING */

    // Decode the center pixel's material from the G-Buffer's encoded ID
    setScene(iMouse);
    int centerID = int(gBuffer.w + 0.5);
    HitInfo centerHit;
    centerHit.objectType  = centerID / 100;
    centerHit.objectIndex = centerID % 100;
    Material centerMat;
    fetchMaterial(centerHit, centerMat);


    /* GETTING PHIs */

    // Base phi variables
    float nPhi = 4.0;
    float dPhi = 10.0;
    float cPhi = 3.0;
    float sizePhi = 0.5;

    // If the center material is a metal, increase each phi based on the roughness
    if (centerMat.type == METAL) {
        nPhi = mix(128.0, nPhi, centerMat.roughness);
        dPhi = mix(32.0, dPhi, centerMat.roughness);
        cPhi = mix(64.0, cPhi, centerMat.roughness);
        sizePhi = mix(0.1, sizePhi, centerMat.roughness);
    }


    /* Kernel & denoise strength */

    // Get the denoise strength, based on sizePhi
    float denoiseStrength = mix(DENOISE_RANGE.x, DENOISE_RANGE.y, sizePhi);

    // 5x5 B-Spline Kernel Weights
    float kernel[25] = float[](
        1.0/256.0,  4.0/256.0,  6.0/256.0,  4.0/256.0, 1.0/256.0,
        4.0/256.0, 16.0/256.0, 24.0/256.0, 16.0/256.0, 4.0/256.0,
        6.0/256.0, 24.0/256.0, 36.0/256.0, 24.0/256.0, 6.0/256.0,
        4.0/256.0, 16.0/256.0, 24.0/256.0, 16.0/256.0, 4.0/256.0,
        1.0/256.0,  4.0/256.0,  6.0/256.0,  4.0/256.0, 1.0/256.0
    );
    
     // Get a random rotation matrix
    float ang = 2.0 * PI * rnd1(seed);
    mat2 m = mat2(cos(ang), sin(ang), -sin(ang), cos(ang));


    /* Kernal execute */

    // Save total accumulation values
    vec3 sumColor = vec3(0.0);
    float sumWeight = 0.0;

    // Accumulate over the 5x5 kernal grid
    for (int y = -2; y <= 2; y++) {
        for (int x = -2; x <= 2; x++) {

            /* SETUP SAMPLE VALUES */

            // Get this sample's coordinate
            vec2 offset = vec2(x, y) * denoiseStrength;
            vec2 sampleCoord = clamp(centerCoord + m*offset, vec2(0), iResolution.xy - vec2(1));
            vec2 sampleUV = sampleCoord / iResolution.xy;

            // Get the G-Buffer values at this sample
            vec4 sampleGBuffer = texture(iChannel0, sampleUV);
            vec3 sampleNormal = unpackNormal(sampleGBuffer.xy);
            float sampleDepth = sampleGBuffer.z;


            /* GET WEIGHTS */

            // Normal
            vec3 t = centerNormal - sampleNormal;
            float dist2 = max(dot(t, t), 0.0);
            float normalWeight = min(exp(-(dist2)*nPhi), 1.0);

            // Depth
            float distDepth = abs(centerDepth - sampleDepth);
            float depthWeight = min(exp(-distDepth * dPhi), 1.0);

            // Color
            vec3 sampleColor = texture(iChannel1, sampleUV).rgb;
            t = centerColor - sampleColor;
            dist2 = max(dot(t, t), 0.0);
            float colorWeight = min(exp(-(dist2)*cPhi), 1.0);

            // Kernel
            float kernelWeight = kernel[(y + 2) * 5 + (x + 2)];


            /* SUM UP VALUES */

            // Total weight
            float totalWeight = colorWeight*normalWeight*depthWeight*kernelWeight;

            // Sum
            sumColor += sampleColor*totalWeight;
            sumWeight += totalWeight;
        }
    }

    // Denoised base color
    vec3 denoisedColor = sumColor/sumWeight;

    /* AABB Varrience Clip */

    vec3 totalColor = vec3(0.0);
    vec3 totalColor2 = vec3(0.0);

    // Sum up surounding colors in a 3x3 grid
    for(int y = -1; y <= 1; y++) {
        for(int x = -1; x <= 1; x++) {
            vec2 offset = vec2(x, y) * denoiseStrength;
            vec2 offsetUV = clamp(centerCoord + offset, vec2(0), iResolution.xy - vec2(1)) / iResolution.xy;
            vec3 neighborColor = texture(iChannel1, offsetUV).rgb;
            
            totalColor += neighborColor;
            totalColor2 += neighborColor * neighborColor;
        }
    }
    
    totalColor += denoisedColor;
    totalColor2 += denoisedColor * denoisedColor;

    // Get the average color and the standard deviation 
    vec3 averageColor = totalColor / 10.0;
    vec3 standardDeviation = sqrt(max(totalColor2 / 10.0 - averageColor * averageColor, 0.0));

    // Get a factor based on the number of pixels moving
    float motionPixels = length(motionVector * iResolution.xy);
    float motionFactor = smoothstep(2.0, 0.0, motionPixels);
    
    // Get a gamma value to expand (or not expand) the aabb box
    float gamma = mix(0.25, 1.25, validHistory ? (historyLength / MAX_HISTORY) : 0.0);
    gamma += (motionFactor * 2.0); // Expands box when still, tightens when moving
    
    // Calculate the aab min & max based on gamma
    vec3 aabbMin = averageColor - standardDeviation * gamma;
    vec3 aabbMax = averageColor + standardDeviation * gamma;
    
    // Include the denoised color in the box
    aabbMin = min(aabbMin, denoisedColor);
    aabbMax = max(aabbMax, denoisedColor);

    // Clamp the history color to reality
    vec3 clippedHistory = clamp(historyColor, aabbMin, aabbMax);

    float nextHistory = min(historyLength + 1.0, MAX_HISTORY);
    float blendWeight = 1.0 - (1.0 / nextHistory);

    if (!validHistory) {
        blendWeight = 0.0;
        nextHistory = 1.0;
    }

    vec3 finalColor = mix(denoisedColor, clippedHistory, blendWeight);
    fragColor = clamp(vec4(finalColor, nextHistory / MAX_HISTORY), 0.0, 1.0);

    #else

    fragColor = texelFetch(iChannel1, ivec2(fragCoord), 0);

    #endif
}
