struct GPUMeshEntry {
    uint baseVertex;
    uint baseIndex;
    uint indexCount;
    uint padding;
};

struct GPUVertex {
    vec4 position;
    vec4 normal;
    vec4 texCoords;
};

struct GPUInstance {
    mat4 transform;
    mat4 invTransform;
    uint meshIndex;
    uint padding1;
    uint padding2;
    uint padding3;
};

