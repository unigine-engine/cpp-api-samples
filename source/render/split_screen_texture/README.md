# Split-Screen Texture

This sample demonstrates how to capture views from two different cameras into separate textures and implement a split-screen layout using the C++ API.

Each camera renders its output to a texture using *Viewport::renderTexture2D()*. These textures are displayed in a vertical split-screen layout using *WidgetSprite* elements, and at the same time are applied to surfaces by assigning them to the albedo slot of static mesh materials. The layout adjusts dynamically to screen size changes.

This setup can be used for multiplayer screen sharing, camera comparisons, or in-game monitors rendered from multiple viewpoints.