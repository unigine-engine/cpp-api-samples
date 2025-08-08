# Procedural Mesh Modification

This sample demonstrates how to dynamically generate, modify, and apply procedural geometry at runtime using different procedural mesh modification methods.

The dynamic surface mesh being built and animated over time using trigonometric functions. The geometry is rebuilt each frame depending on current configuration.

You can experiment with various procedural modes (such as *Dynamic, File*, or *Blob*), as well as configure how geometry is stored and accessed by selecting different MeshRender usage flags (DirectX 12 only). These flags determine whether vertex and/or index data is kept in RAM instead of VRAM, allowing the GPU to render directly from system memory.

Additionally, you can choose whether mesh generation occurs on the **Main** thread or in the **Background**, giving you more control over performance and responsiveness during updates.

+**Dynamic** - fastest performance, stored in RAM and VRAM, not automatically unloaded from memory.
+**Blob** - moderate performance, stored in RAM and VRAM, automatically unloaded from memory.
+**File** - slowest performance, all data stored on disk, automatically unloaded from memory.

You can also toggle between different update modes (*Async* or *Force*), choose memory transfer strategies (*Copy* or *Move*), and optionally enable manual control of the *MeshRender*, where you update its content explicitly after modifying mesh data, instead of relying on automatic updates. Additionally, collision data can be generated explicitly after geometry modification, as it is not created automatically.

Use this sample to understand how procedural mesh modification works across different configurations and explore how update strategies influence performance.

Refer to the Procedural Mesh Workflow article for more details on memory behavior, update strategies, and best practices when working with procedural geometry.