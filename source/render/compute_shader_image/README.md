# Compute Shader Image

This sample demonstrates how to create a texture at runtime and update it entirely on the GPU using compute shaders with read-write (unordered) access. The 2D texture is dynamically created and assigned to a material's albedo slot.
Each frame, the compute shader updates the texture by using simulation parameters frame time (ifps) and dispatching GPU threads in 32&#215;32 groups. Then the shader modifies the texture content using unordered access, with all calculations and writes handled fully on the GPU. The CPU is only responsible for initial setup and parameter updates.

The compute shader is used to perform custom operations on textures, allowing for real-time image manipulation or procedural content generation.