# Gbuffer Read

This sample demonstrates how to access *G-buffer* textures at different stages of the rendering process by configuring a custom *Viewport* and intercepting its output at the *G-buffer* rendering stage.

A temporary viewport is used to render a selected node with a simplified pipeline to stop rendering after the *G-buffer* is filled. When the rendering reaches this stage, a callback is triggered to fetch *G-buffer* textures such as depth, albedo, normals, metalness, and roughness. These textures are then copied to user-defined render targets and displayed on screen planes using custom materials.

This setup is useful for debugging, material analysis, or developing post-processing effects that require direct access to the intermediate data in the rendering pipeline.