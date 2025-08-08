# Camera to Texture

This sample shows how to capture the output of a camera in real time and project it onto a material's albedo texture using *Viewport::renderTexture2D()*.
A renderable 2D texture is created and set as the albedo texture of the material assigned to the plane. Each frame, the active camera's view is rendered into this texture, updating the appearance of the object it's applied to. Texture sampling settings like linear filtering and anisotropy are configured to ensure visual quality. The UV transform is also adjusted to correct for platform-dependent flipping.

This method can be used for features such as security monitors, live camera feeds, dynamic mirrors, portals, or mini-maps in your worlds.