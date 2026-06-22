// SSBO buffers
layout(std430, binding = 0) readonly buffer MeshEntryBuffer { GPUMeshEntry meshEntries[]; };
layout(std430, binding = 1) readonly buffer VertexBuffer { GPUVertex vertices[]; };
layout(std430, binding = 2) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 3) readonly buffer InstanceBuffer { GPUInstance instances[]; };
layout(std430, binding = 4) readonly buffer MaterialBuffer { Material materials[]; };

// Total instance count
uniform int uInstanceCount;
