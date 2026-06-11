HitInfo rayTriangle(Ray ray, vec3 v0, vec3 v1, vec3 v2) {
    HitInfo hit;
    hit.hit = false;

    const vec3 edge1 = v1 - v0;
    const vec3 edge2 = v2 - v0;
    const vec3 pvec = cross(ray.direction, edge2);
    const float det = dot(edge1, pvec);

    if (abs(det) < EPSILON) return hit;

    const float invDet = 1.0 / det;
    const vec3 tvec = ray.origin - v0;
    const float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return hit;

    const vec3 qvec = cross(tvec, edge1);
    const float v = dot(ray.direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return hit;

    const float t = dot(edge2, qvec) * invDet;

    if (t > EPSILON) {
        hit.hit = true;
        hit.dist = t;
        hit.hitPos = ray.origin + ray.direction * t;
        hit.normal = normalize(cross(edge1, edge2));
    }

    return hit;
}

HitInfo rayScene(Ray ray) {

    // Create a closestHit with temp values meant to be overwritten
    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.dist = REALLY_FAR;

    // Loop every instance
    for (int instID = 0; instID < u_instanceCount; instID++) {

        // Get the current instance
        const GPUInstance instance = instances[instID];

        // Get a new ray in local space
        vec3 localRayOrigin = (instance.invTransform * vec4(ray.origin, 1.0)).xyz;
        vec3 localRayDirection = normalize((instance.invTransform * vec4(ray.direction, 0.0)).xyz);
       
        // Construct the ray and save it's length
        const Ray localRay = Ray(localRayOrigin, localRayDirection);
        const float rayLength = length(localRayDirection);

        // Get the current mesh entry
        const GPUMeshEntry entry = meshEntries[instance.meshIndex];

        // Loop each index, skip by three (three verts per triangle)
        for (uint i = 0; i < entry.indexCount; i += 3) {

            // Get each index
            const uint idx0 = indices[entry.baseIndex + i + 0];
            const uint idx1 = indices[entry.baseIndex + i + 1];
            const uint idx2 = indices[entry.baseIndex + i + 2];

            // Get each vertex
            const vec3 v0 = vertices[entry.baseVertex + idx0].position.xyz;
            const vec3 v1 = vertices[entry.baseVertex + idx1].position.xyz;
            const vec3 v2 = vertices[entry.baseVertex + idx2].position.xyz;

            // Test the ray-triangle intersection for this triangle
            HitInfo currentHit = rayTriangle(localRay, v0, v1, v2);

            // Re-scale distance back to World Space
            currentHit.dist /= rayLength;

            // See if it's closer or not
            if (currentHit.hit && currentHit.dist < closestHit.dist) {

                // If it is, set this as the closest hit
                closestHit = currentHit;
                
                // Transform hitPos & normal back into world space
                closestHit.hitPos = (instance.transform * vec4(currentHit.hitPos, 1.0)).xyz;
                closestHit.normal = normalize((vec4(currentHit.normal, 0.0) * instance.invTransform).xyz);
            }
        }
    }

    // Return the closest hit found
    return closestHit;
}

