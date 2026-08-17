vec3 colorScene(const in Ray cameraRay) {return vec3(0.0);}

void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    // Anti-aliasing
    vec2 jitter = vec2(0.0);

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
    //setScene();

    vec3 col = colorScene(ray);
    fragColor = vec4(col, 1.0);

}
