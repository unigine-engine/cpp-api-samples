# Textures

This sample demonstrates how to create and update procedural textures on static meshes in real time using the C++ API.

Each mesh is assigned a custom-generated image, which is updated every frame using a time-based color pattern. The pixel data is written manually into an *Image* class instance, and the result is applied to the albedo texture slot of the mesh's material using *Material::setTextureImage()*.

This method allows generating dynamic patterns, noise, or other procedural visuals without using compute shaders or external image files.