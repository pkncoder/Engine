---
domain: GPU
program: api
level: medium
structure: interface
tags:
  - renderGraphNode
---
---

## Definition

The [[IShader]] class controls all workings with the OpenGL shaders and programs. It also connects any non-uniform buffer uniforms. The IShaderNode is a visual of the [[IShader]] class.
## Ports

![[ShaderNodePorts]]

The *source* port takes in the pre-compiled & pre-processed (stitched) file.
The *type* port defines the shader type (Compute, Vertex, etc.)