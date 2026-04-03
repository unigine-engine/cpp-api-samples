# Mesh To Mask Texture

This sample demonstrates how to render a mesh into a texture using a custom material, producing an image to be used as a mask. The mesh is rendered from a dedicated camera view directly into a texture, which is then assigned to the albedo slot of a preview material.

Use the manipulator to move the camera view and observe how different viewing angles affect the generated texture mask.

The sample compares two rendering approaches:

-**Renderer.RenderMesh** - standard camera-based rendering **(recommended for most use cases)**. The Engine automatically prepares matrices, render states, and shaders, so you only need to provide the mesh, material, transform, and camera.
-**MeshRender.Render** - low-level, **fully manual** projection and modelview setup. This method provides finer control over the rendering pipeline but requires explicit setup of projection and view matrices, RenderState configuration, and manual shader parameter management.

Both methods produce identical visual results, allowing you to choose whichever approach best fits your workflow.

This technique is useful for dynamic decals, procedural effects, or any feature requiring real-time texture generation from 3D geometry.