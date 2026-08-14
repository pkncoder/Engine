
#define AABB_VARIENCE_CLIP_SIGMA_MODIFICAITON 1.5
#define REPROJECTION_MAX_ACCUM_FRAMES 64.0
#define REPROJECTION_MAX_CAMERA_ACCUM_FRAMES 1.0

/* ----------------- Helpers ----------------- */

// Get the local basis matrix
mat3 getBasis(vec3 n) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    return mat3(t, b, n);
}

// MIS pdf weighting
float powerHeuristic(float a, float b) {
    float a2 = a * a;
    float b2 = b * b;
    return a2 / (a2 + b2);
}



/* ----------------- Multiple Importance Sampling (MIS) ----------------- */

// Solid-Angle light sampling
LightSample sampleSphereLight(
    const in Sphere light,
    const vec3 hitPos,
    inout uint seed
) {
    // Initialize a light sample so we can pass around values
    LightSample lightSample;

    // Get distance to the light center
    vec3 toLightCenter = light.origin - hitPos;
    float dist2 = dot(toLightCenter, toLightCenter);
    float dist = sqrt(dist2);
    toLightCenter = toLightCenter / dist; // Normalize light center direction

    // Raidus squared
    float r2 = light.radius * light.radius;

    // Check to make sure the point isn't inside the light
    if (dist2 <= r2) { 
        lightSample.pdf = 0.0;
        return lightSample;
    }

    // Find out how large the cone will be to sample from
    float cosThetaMax = sqrt(1.0 - r2 / dist2);

    // Sample randomly (and uniformly) within the cone
    float u1 = rnd1(seed);
    float u2 = rnd1(seed);

    // Map the uniform samples to the cone
    float cosTheta = 1.0 - u1 + u1 * cosThetaMax;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float phi = 2.0 * PI * u2;

    // Get the direction of the new sample in relation to the cone
    vec3 localConeDir = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

    // Get the basis mat
    mat3 basis = getBasis(toLightCenter);

    // Assign the LightSample values
    lightSample.direction = basis * localConeDir;
    lightSample.radiance = light.material.emmisive;
    lightSample.dist = dist;

    // Set the PDF of the sample (1 in the area of the cone)
    lightSample.pdf = INV_TWO_PI / (1.0 - cosThetaMax);

    // Return the final lightSample
    return lightSample;
}

// Function to just get the pdf of the solid-angle light sample
float getSphereLightPdf(
    const in Sphere light,
    vec3 prevHitPos
) {
    // Get distance (squared) to the center of the light + the squared radius
    vec3 toLightCenter = light.origin - prevHitPos;
    float dist2 = dot(toLightCenter, toLightCenter);
    float r2 = light.radius * light.radius;

    // Safety check for being inside the sphere
    if (dist2 <= r2) return 0.0;

    // Get the size of the cone
    float cosThetaMax = sqrt(1.0 - r2 / dist2);

    // Return the final pdf (1 in area of the cone)
    return INV_TWO_PI / (1.0 - cosThetaMax);
}

// Get the pdf for our diffuse model
float diffusePDF(vec3 normal, vec3 dir) {
    return max(dot(normal, dir), 0.0) * INV_PI;
}

// Cosine-weighted hemisphere sampling
vec3 sampleDiffuse(vec3 normal, out float pdf, inout uint seed) {
    float u1 = rnd1(seed);
    float u2 = rnd1(seed);
    float r = sqrt(u2);
    float phi = TWO_PI * u1;

    vec3 localDir = vec3(r * cos(phi), r * sin(phi), sqrt(max(0.0, 1.0 - u2)));
    pdf = localDir.z * INV_PI;

    return getBasis(normal) * localDir;
}



/* ----------------- Bi-Directional Reflectance Function (BDRF) ----------------- */

// GGX Microfacet sampling
vec3 sampleGGXWorld(vec3 normal, float roughness, float u1, float u2) {
    float a = roughness * roughness;
    float phi = TWO_PI * u1;

    float cosTheta = sqrt(max((1.0 - u2) / max(1.0 + (a * a - 1.0) * u2, EPSILON), 0.0));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

    vec3 localH = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return getBasis(normal) * localH;
}

// Microfacet Masking-Shadowing Function (Smith-GGX)
float smithGeometry(float NdotV, float NdotL, float roughness) {
    float a = max(roughness * roughness, EPSILON);
    float k = a * 0.5;
    float g1v = NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
    float g1l = NdotL / max(NdotL * (1.0 - k) + k, EPSILON);
    return g1v * g1l;
}

// Fresnel
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    float t = clamp(1.0 - cosTheta, 0.0, 1.0);
    float t2 = t * t;
    return F0 + (vec3(1.0) - F0) * (t2 * t2 * t);
}



/* ----------------- Ray Interactions ----------------- */

// Modify values that need to be changed for the bounce
void processBounceStep(
    inout Ray ray,
    const in HitInfo hit,
    const in Material mat,
    inout vec3 colorMult,
    out float savedBouncePdf,
    out bool savedBounceSpecularAction,
    inout uint seed
) {

    // Save the view and normal vectors
    vec3 V = -ray.direction;
    vec3 N = hit.normal;

    // Lambertion diffuse
    if (mat.type == LAMBERTION) {

        // Sample the diffuse pdf
        float pdf;
        vec3 rayDirection = sampleDiffuse(N, pdf, seed);

        // Update the ray information
        ray.origin = hit.hitPos + N * EPSILON;
        ray.direction = rayDirection;

        // Modify the throughput
        colorMult *= mat.color;

        // Set the saved values
        savedBouncePdf = pdf;
        savedBounceSpecularAction = false;
    }

    // Specular diffuse
    else if (mat.type == METAL) {

        // Perfect reflection (or close enough)
        if (mat.roughness < 0.01) {

            // Perfectly reflect the ray
            vec3 rayDirection = reflect(-V, N);

            // Update the ray information
            ray.origin = hit.hitPos + N * EPSILON;
            ray.direction = rayDirection;

            // Modify the throughput
            colorMult *= fresnelSchlick(max(dot(N, rayDirection), 0.0), mat.color);

            // Set the saved values
            savedBouncePdf = 1.0;
            savedBounceSpecularAction = true;
        }

        // Non-perfect reflections
        else {

            // Sample GGX
            float u1 = rnd1(seed);
            float u2 = rnd1(seed);
            vec3 H = sampleGGXWorld(N, mat.roughness, u1, u2);

            // Get NdotL to check for a ray not being visable
            vec3 rayDirection = reflect(-V, H);
            float NdotL = dot(N, rayDirection);
            if (NdotL <= 0.0) { colorMult = vec3(0.0); return; }

            // Update ray information
            ray.origin = hit.hitPos + N * EPSILON;
            ray.direction = rayDirection;

            // Values for the next BDRF calculations
            float NdotV = max(dot(N, V), EPSILON);
            float NdotH = max(dot(N, H), EPSILON);
            float HdotV = max(dot(H, V), EPSILON);

            // Get the G & F components
            float G = smithGeometry(NdotV, NdotL, mat.roughness);
            vec3 F = fresnelSchlick(HdotV, mat.color);

            // Get the D component
            float a = mat.roughness * mat.roughness;
            float a2 = a * a;

            // Update the color mult
            colorMult *= (F * G * HdotV) / (NdotV * NdotH);

            // Calculate the pdf
            float denomPdf = NdotH * NdotH * (a2 - 1.0) + 1.0;
            float specularPdf = (a2 * INV_PI * NdotH) / (4.0 * (denomPdf * denomPdf) * HdotV);

            // Set the saved values
            savedBouncePdf = specularPdf;
            savedBounceSpecularAction = false;
        }
    }
}



/* ----------------- Coloring Functions ----------------- */

// Sky color
vec3 colorSky(Ray ray) {
    vec3 sunDir = normalize(vec3(0.4, 0.8, 0.3));
    float sunDot = max(dot(ray.direction, sunDir), 0.0);

    // Horizon/zenith gradient
    float t = max(ray.direction.y, 0.0);
    vec3 sky = mix(vec3(0.7, 0.85, 1.0), vec3(0.15, 0.35, 0.7), pow(t, 0.6));

    // Sun disc + halo
    sky += vec3(1.2, 1.0, 0.6) * pow(sunDot, 128.0) * 8.0;  // disc
    sky += vec3(0.9, 0.7, 0.4) * pow(sunDot, 4.0)   * 0.4;  // halo

    return sky * SKYBOX_COLOR_MULT;
}

// Entire scene coloring (render() function)
vec3 colorScene(const in Ray cameraRay, vec2 fragCoord) {

    // Set higher-scope structs
    Ray ray = cameraRay;
    Material material;
    HitInfo hit;

    // Color & throughput
    vec3 color = vec3(0.0);
    vec3 colorMult = vec3(1.0);

    // Saved previous bounce things
    float prevBouncePdf = 1.0;
    bool prevBounceSpecular = false;
    vec3 prevHitPos = ray.origin;

    for (int bounce = 0; bounce < MAX_BOUNCES; bounce++) {

        // Setup & hit the scene normally if not first bounce
        if (bounce != 0) {
            hit = rayScene(ray, seed);

            // Check for hit
            if (!hit.hit) {
                color += colorSky(ray) * colorMult; // No hit-sky
                break;
            }

            // Set the material
            fetchMaterial(hit, material);
        } else { // First bounce (use G-Buffer)
            // Setup the scene hit with the G-Buffer
            if (!unpackGBuffer(ray, hit, material, iChannel0, fragCoord)) {
                color += colorSky(ray) * colorMult;
                break;
            }
        }

        #if (SPH_LIGHT_NUM > 0)
        // Check if current hit is emmisive
        if (length(material.emmisive) > 0.0) {
            if (bounce == 0 || prevBounceSpecular || MIS == 0) { // First bounce, specular, or no MIS, flat color mult
                color += material.emmisive * colorMult;
            } else { // Else, update pdf information & weight the color output
                float pickProb = getLuma(material.emmisive) / max(scene.totalEmmisiveValue, EPSILON);
                float lightPdf = getSphereLightPdf(scene.spheres[hit.objectIndex], prevHitPos) * pickProb;
                float misWeight = powerHeuristic(prevBouncePdf, lightPdf);
                color += material.emmisive * colorMult * misWeight;
            }
            break;
        }

        #if (MIS == 1)
        // Stoastic light selection
        if (SPH_LIGHT_NUM > 0 && (material.type != METAL || material.roughness > 0.01)) {
            float randomVal = rnd1(seed) * scene.totalEmmisiveValue;
            float currentSum = 0.0;
            int randomLightIndex = 0;

            // Select by the total level of light in the room
            for (int i = 0; i < SPH_LIGHT_NUM; i++) {
                currentSum += scene.emmisiveValues[i];
                if (randomVal <= currentSum || i == SPH_LIGHT_NUM - 1) {
                    randomLightIndex = i;
                    break;
                }
            }

            // Get the selected light and sample light
            Sphere light = scene.spheres[randomLightIndex];
            float pickProb = getLuma(light.material.emmisive) / max(scene.totalEmmisiveValue, EPSILON);
            LightSample ls = sampleSphereLight(light, hit.hitPos, seed);

            // Get the shadow ray
            Ray shadowRay = Ray(hit.hitPos + hit.normal * EPSILON, ls.direction);

            // Check the for shadow hit (or specifically not)
            if (!rayHit(shadowRay, ls.dist - EPSILON, randomLightIndex)) {

                // Check to see if the hit pos can see the light
                float NdotL = max(dot(hit.normal, ls.direction), 0.0);
                if (NdotL > 0.0) { // Calculate the BDRF

                    // Save the view direction
                    vec3 V = -ray.direction;
                    vec3 brdfDirect = vec3(0.0);

                    // Save the final pdf
                    float brdfPdf = 0.0;

                    if (material.type == LAMBERTION) { // Basic lambertion diffuse
                        brdfDirect = material.color * INV_PI;
                        brdfPdf = NdotL * INV_PI;
                    }
                    else if (material.type == METAL) { // Full specular GGX setup

                        // Save the H vector
                        vec3 H = normalize(V + ls.direction);

                        // Get the NdotX vectors
                        float NdotV = max(dot(hit.normal, V), EPSILON);
                        float NdotH = max(dot(hit.normal, H), EPSILON);
                        float HdotV = max(dot(H, V), EPSILON);

                        // Calculate DGF
                        float a = material.roughness * material.roughness;
                        float a2 = a * a;
                        float denomD = NdotH * NdotH * (a2 - 1.0) + 1.0;
                        float D = (a2 * INV_PI) / (denomD * denomD);
                        float G = smithGeometry(NdotV, NdotL, material.roughness);
                        vec3 F = fresnelSchlick(HdotV, material.color);

                        // Set the bdrf values
                        brdfDirect = (D * G * F) / max(4.0 * NdotV * NdotL, 1e-5);
                        brdfPdf = (D * NdotH) / max(4.0 * HdotV, 1e-5);
                    }

                    // Make sure that the chance was greater than 0
                    if (brdfPdf > 0.0) {

                        // Get the misWeight and add the contribution
                        float combinedLightPdf = ls.pdf * pickProb;
                        float misWeight = powerHeuristic(combinedLightPdf, brdfPdf);
                        color += colorMult * brdfDirect * ls.radiance * NdotL * misWeight / max(combinedLightPdf, EPSILON);
                    }
                }
            }
        }
        #endif
        #endif

        // Set the previous value
        prevHitPos = hit.hitPos;

        // Process bounce
        processBounceStep(ray, hit, material, colorMult, prevBouncePdf, prevBounceSpecular, seed);

        // Russian roulette
        if (bounce > MIN_BOUNCE_RUSSIAN_ROULETTE) { // Don't do it too early
            // Find the largest color weight
            float p = max(colorMult.r, max(colorMult.g, colorMult.b));

            // If the random chance is bigger than the largest color weight
            if (rnd1(seed) > p)
                break;

            // Account for lost energy
            colorMult /= p;
        }
    }

    // Return the final color
    return color;
}



/* Main / Setup Functions */

void mainImage( out vec4 fragColor, in vec2 fragCoord ) {

    // Store the iMouse from this frame for the next to get it's camera state
    if (ivec2(fragCoord) == ivec2(0, 0)) {
        fragColor = vec4(iMouse.xy / iResolution.xy, 0.0, 0.0);
        return;
    }

    // Get the current seed to the same values as pass one (for jitter)
    setSeed(fragCoord, uint(iFrame), iChannel3);

    // Calculate the uv and camera FOV
    #if (ANTI_ALIASING == 0)
    vec2 jitter = vec2(0.0);
    #else
    vec2 jitter = halton(iFrame % 16) - 0.5;
    #endif

    // Get uv and FOV distance
    vec2 uv = ((fragCoord + jitter) / iResolution.xy * 2.0 - 1.0) * vec2(iResolution.x / iResolution.y, 1.0);
    float cameraDist = 1.0f / tan(FOV * 0.5f * PI / 180.0f); 

    // Get camera state
    vec3 cameraPos, cameraFwd, cameraUp, cameraRight;
    getCamereaState(iMouse.xy, iResolution.xy, cameraPos, cameraFwd, cameraUp, cameraRight);

    // Get the initial ray
    vec3 rayDirection = normalize(vec3(uv, cameraDist));
    rayDirection = normalize(mat3(cameraRight, cameraUp, cameraFwd) * rayDirection);
    Ray ray = Ray(
        cameraPos,
        rayDirection
    );

    // Get the scene
    setScene(iMouse);
    #if (SCENE==4)
    //scene.spheres[0].origin = vec3(sin(iTime/0.25)*4.0, 2.0, cos(iTime/0.25)*4.0 + (3.0 + 2.0*sqrt(3.0)/3.0));
    #endif
    
    // Save a color at a higher scope to be added to each ray sample
    vec3 col = vec3(0.0);

    // Loop each sample pass
    for (int i = 0; i < SAMPLES; i++) {
        col += colorScene(ray, fragCoord);
    }

    // Average up the values
    col /= float(SAMPLES);
    
    // Firefly clamp
    float maxLuminance = 10.0;
    float currentLuma = dot(col, vec3(0.2126, 0.7152, 0.0722));
    if (currentLuma > maxLuminance) {
        col *= maxLuminance / currentLuma;
    }

    // Per-color clamp as a second line of defence
    col = min(col, vec3(maxLuminance));

    // Temporal Reprojection (or not)
    #if (TEMPORAL_REPROJECTION == 1)

    // Get last frame's mouse pos to reconstruct the previous frame's camera state
    vec2 prevMouseNorm = texelFetch(iChannel1, ivec2(0, 0), 0).xy;
    vec2 prevMouse = prevMouseNorm * iResolution.xy; 

    // Save to modifyvalues for final color
    vec3  blendedColor = col;   // Default: no history, use raw sample
    float newAlpha     = 1.0;

    // Get the hit depth
    float gBufferDepth = texelFetch(iChannel0, ivec2(fragCoord), 0).z;

    // No-hit, return unblended color
    if (gBufferDepth > SUPER_FAR) { fragColor = vec4(blendedColor, newAlpha); return; }

    // Reconstruct the worldPos, and then the UV from the previous frame
    vec3 worldPos = reconstructWorld(fragCoord, gBufferDepth, iMouse.xy, iResolution.xy);
    vec2 prevUV   = projectToScreen(worldPos, prevMouse, iResolution.xy);

    // Make sure that the previousUV is on screen, and isn't in the meta data corner
    bool isValidUV = prevUV.x > 0.0 && prevUV.x < 1.0 && // X Bounds
                   prevUV.y > 0.0 && prevUV.y < 1.0 && // Y Bounds
                   prevUV.x > (1.5 / iResolution.x); // Metadata Corner

    if (!isValidUV) { fragColor = vec4(blendedColor, newAlpha); return; }

    // Get the last color & alpha
    vec4 histSample = texture(iChannel1, prevUV);
    vec3 histColor  = histSample.rgb;
    float prevAlpha = histSample.a;

    // Save total values that will be averaged at the end of the AABB varience clip
    vec3 averageColor  = vec3(0.0);
    vec3 averageColor2 = vec3(0.0);

    // AABB Varience Clip
    for (int vy = -1; vy <= 1; vy++) { // Loop Y
        for (int vx = -1; vx <= 1; vx++) { // Loop X
            // Clamp to resolution; start at x=1 to skip metadata pixel
            ivec2 sampleCoord = clamp(ivec2(fragCoord) + ivec2(vx, vy),
                             ivec2(1, 0), ivec2(iResolution.xy) - 1);
            vec3 color  = texelFetch(iChannel1, sampleCoord, 0).rgb;
            averageColor   += color;
            averageColor2  += color * color;
        }
    }

    // Average the final summations
    averageColor  /= 9.0;
    averageColor2 /= 9.0;

    // Get the deviation
    vec3 sigma = sqrt(max(averageColor2 - averageColor * averageColor, vec3(0.0)));

    // Modify the min & max of the AABB with the deviation
    vec3 aabbMin = averageColor - AABB_VARIENCE_CLIP_SIGMA_MODIFICAITON * sigma;
    vec3 aabbMax = averageColor + AABB_VARIENCE_CLIP_SIGMA_MODIFICAITON * sigma;

    // Clamp
    histColor = clamp(histColor, aabbMin, aabbMax);

    // Get the previous amount of frames accumulated
    float prevN = (prevAlpha > 0.0) ? (1.0 / prevAlpha) : 0.0;

    // Reduce the amount of frames to acumulate when the camera is moving
    bool isCameraMoving = (iMouse.z > 0.0);
    if (isCameraMoving) prevN = min(prevN, REPROJECTION_MAX_CAMERA_ACCUM_FRAMES);

    // Calculate the new alpha by adding one to the prevN (and restricting to the max frames)
    newAlpha = 1.0 / min(prevN + 1.0, REPROJECTION_MAX_ACCUM_FRAMES);

    // Mix together the last frame and this frame
    blendedColor = mix(histColor, col, newAlpha);

    // Final color
    fragColor = vec4(blendedColor, newAlpha);

    #elif (ACCUM == 1)
    vec4 lastFrameColor = texture(iChannel1, fragCoord / iResolution.xy);
    float blend = (iMouse.z > 0.0 || lastFrameColor.a == 0.0) ? 1.0 : 1.0 / (1.0 + (1.0 / lastFrameColor.a));
    fragColor = vec4(mix(lastFrameColor.rgb, col, blend), blend);
    #else
    fragColor = vec4(col, 0.0);
    #endif
}

