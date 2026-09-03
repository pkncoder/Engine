---
program: api
domain: GPU
level: low
structure: interface
tags:
  - renderGraphNode
---
---

## Definition

The IShaderPassNode is an interface for each shader node that is appart of the [[RenderGraph]].

## Ports

Each node includes a varriety of *ports* connection points that may or may not allosw for multiple connections. These tranlate to public variables inside of the program.

The base IShaderPassNode exposes these ports:
![[ShaderPassNodePorts|200]]

The *inputs* port may in mutiple different [[IShaderInput]] nodes.
The *outputs* port exposes mutiple different [[IShaderOutput]] nodes depending on the child class.
The *shader* port must have an [[ShaderNode]] depending on it's node type. 

> [!info] Note
> The *shader* port will be private on child classes of IShaderPassNode. In the case of custom IShaderNodes, the port will be exposed.

## Implementation

As the IShaderNode is an interface, it exposes *virtual* - implemented by child class - and *inherited* - defined by parent - functions & attributes.

Here is a visual of what IShaderPassNode sets up for child classes:
![[ShaderPassNodeImplementation|250]]

The *execute()* function is called by an outside class, and runs the [pipeline](#pipeline) for that IShaderPassNode.
The *drawCommand* array is wiped frame-to-frame, and assists in the pipeline state

## Pipeline