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

The IShaderOutput is the minimum needs for any type of output of a shader. Commonly, it will be an [[IGPUTexture]], allowing for it to be blited or used in another shader pass.

## Ports

The IShaderOutput base exposes a number of ports based on the uses for OpenGL. Here is the visual for the IShaderOutput node:
![[IShaderOutputPorts|300]]

The *width* port defines the width of the image.
The *height* port defines the height of the image.
The *channels* port defines the number of texture channels (r, g, b, a) that there are.
The *data* port takes in a std::vector\<unsigned char\> for raw pixel data.

## Implementation

IShaderOutput exposes multiple attributes and functions in each child class for backend use.
![[IShaderOutputImplementation|300]]

The *bind()* function takes in the *type*, *binding*, and *id* attributes to bind the final the final image to the GPU.
The *id* attribute containes the generated OpenGL texture id.
The *type* attribute holds the OpenGL texture type.
The *bindingIndex* is the index the texture is bound to in each shader.