---
domain: GPU
program: api
level: medium
structure: interface
tags:
  - "#renderGraphNode"
---
---

## Definition

The IShaderInput interface is the minimum requirements for any input into any render graph node. It is required for polymorphism as well.

## Ports

IShaderInput exposes no base ports, as it is handled by child classes.
## Implementation

IShaderInput includes these backend attributes and functions for use in child classes
![[IShaderInputImplementation|300]]

The *bind()* function is a required override by child classes, binding whatever input to the GPU.
