# Render Target

This sample demonstrates how to draw directly into a texture using the *RenderTarget* class. The system initializes multiple textures, including a background layer and a dynamic result texture, which then is assigned to the object's material. The *RenderTarget* is used to redirect rendering output from the main framebuffer to a texture, allowing full control over how sprites are composited. Each draw operation includes a UV-based transform to place and scale the sprite, and uses a custom material with direct access to the involved textures. After each draw, the result is blended with the background and mipmaps are generated for correct appearance at various distances. Sprites appear both on mouse clicks and automatically at random positions on the wall surface.

Render states are saved and restored around each operation to isolate the off-screen rendering flow from the rest of the pipeline.

This technique is suitable for creating effects like decal placement, user-driven painting, hit markers, or procedural texture overlays in real time.