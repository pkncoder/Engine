void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    // Anti-aliasing
    #if (ANTI_ALIASING == 0)
    vec2 jitter = vec2(0.0);
    #else
    vec2 jitter = halton(iFrame % 16) - 0.5;
    #endif

    // Set the uv & FOV
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

    // Set the scene & get the first hit
    setScene(iMouse);
    HitInfo hit = rayScene(ray, seed);

    // Check for hit, if hit, pack the normal (xy), hit distance (z), object type + object index (w)
    if (hit.hit) {
        vec2 packedNorm = packNormal(hit.normal);
        float encodedID = float(hit.objectType) * 100.0 + float(hit.objectIndex);

        fragColor = vec4(packedNorm, hit.dist, encodedID);
    } else { // If not, set null values
        // Sky / Miss value
        fragColor = vec4(0.0, 0.0, SUPER_FAR, -1.0);
    }
}
