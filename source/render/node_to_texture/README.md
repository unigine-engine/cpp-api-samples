# Node to Texture

This sample demonstrates how to render a specific node and its children into a texture using *Viewport::renderNodeTexture2D()*. Instead of rendering the whole scene, a custom viewport captures only the target node as seen from a camera, and outputs the result into a 2D texture.
The texture is then assigned as the material's albedo texture. To create clean output, the viewport is configured to skip transparent objects, velocity buffers, post-effects, and debugging visualizers. Lighting is inherited from the world, and the environment is overridden to a black cubemap to prevent background influence.

This sample can be useful for rendering character previews, dynamic item thumbnails, or isolated object views directly onto surfaces.