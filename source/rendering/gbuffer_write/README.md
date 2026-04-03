# Gbuffer Write

This sample demonstrates how to modify *G-buffer* textures at different stages of the rendering process by injecting a custom material at the end of the *G-buffer* pass.

An event handler is registered using *getEventEndOpacityGBuffer()*, which is triggered once all *G-buffer* textures (albedo, normal, etc.) are populated. In this callback, a custom post-material is applied to modify the contents of these textures using dynamic parameters such as influence, plasticity, and color. Temporary render targets are used to perform intermediate writes, and the modified textures are then swapped back into the pipeline.

This technique allows real-time manipulation of *G-buffer* data during the rendering process, enabling custom surface effects or advanced material pre-processing.