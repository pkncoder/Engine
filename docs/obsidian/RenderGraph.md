---
program: api
domain: GPU
level: high
structure: class
---
---
## Description

The render graph is a utility for the final image creation. It helps by:
- Exposing a simple node-based editor
- Giving a simple visual for how the final image is shown
- Creating a high-level wrapper of lower-level nodes & implementations

For a single line definition: *The Render Graph is an API meant to simplify and provide structure to a final renderer.*

## Implementation

The render graph tree contains a variety of different nodes that are meant to connect to eachother. These include:
- [[IShaderPassNode]] - The base node sent for rendering
- [[IShaderInput]] - The input node for the [[IShaderPassNode]]
- [[IShaderOutput]] - The base output node for the [[IShaderPassNode]]
- [[IRenderOutput]] - Sends blitable images to the [[BlitList]]