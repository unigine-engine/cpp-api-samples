# Procedural Mesh Apply

This sample demonstrates the correct order of operations for modifying and applying procedural mesh data at runtime. It serves as a minimal example showing how to build geometry, configure procedural mode, and apply changes to an *ObjectMeshCluster*. The same workflow and apply methods are also available for other objects that support procedural meshes, such as *ObjectMeshStatic*, *ObjectGuiMesh*, *DecalMesh*, and *ObjectMeshClutter*.

Inside the component's code, you'll find a linear implementation of the procedural mesh pipeline, demonstrating how geometry is generated and applied in the correct order.

Use this sample to understand the basic flow of procedural mesh updates and try adjusting configurations to find what best suits your needs.

Refer to the Procedural Mesh Workflow article for more details on memory behavior, update strategies, and best practices when working with procedural geometry.