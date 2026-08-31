---
domain: GPU
program: api
level: high
structure: interface
tags:
  - renderGraphNode
---
---

## Definition

The IRenderOutput node allows for any [[IGPUTexture]] to be sent to the [[BlitList|blit list]].

## Ports

The IRenderOutput has a set of base ports to allow custom configuration:
![[IRenderOutputPorts]]

The *textureName* sets the name of the texture in the blit list.
The *texture* port is the connecting output.