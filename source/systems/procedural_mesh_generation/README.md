# Procedural Mesh Generation

This sample demonstrates how to generate static mesh geometry at runtime using different procedural mesh generation methods. A grid of *ObjectMeshStatic* objects is created, each receiving its geometry from a user-defined callback that builds a box-shaped mesh surface via the *Mesh API*.

You can experiment with various procedural modes (such as *Dynamic, File*, or *Blob*), as well as configure how geometry is stored and accessed by selecting different MeshRender usage flags (DirectX 12 only). These flags determine whether vertex and/or index data is kept in RAM instead of VRAM, allowing the GPU to render directly from system memory.

+**Dynamic** - fastest performance, stored in RAM and VRAM, not automatically unloaded from memory.
+**Blob** - moderate performance, stored in RAM and VRAM, automatically unloaded from memory.
+**File** - slowest performance, all data stored on disk, automatically unloaded from memory.

The **Field Size** parameter defines how many mesh objects are generated along each axis, forming a square grid.

For each configuration, the sample shows total RAM and VRAM usage, along with the number of active mesh objects. This makes it easier evaluate the performance, memory layout, and behavior of each procedural mode in different runtime conditions.

Use this sample to understand how procedural mesh generation works across different modes, observe how geometry is stored and managed between RAM, VRAM, and disk, profile memory usage for small versus large number of procedural objects, and explore how update strategies influence performance.

Refer to the Procedural Mesh Workflow article for more details on memory behavior, update strategies, and best practices when working with procedural geometry.