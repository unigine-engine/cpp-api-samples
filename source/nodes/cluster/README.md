# Cluster

This sample demonstrates dynamic manipulation of **ObjectMeshCluster** in UNIGINE, showcasing how to add/remove mesh instances at runtime through user interaction. A **Mesh Cluster** allows you to bake identical meshes (with the same material applied to their surfaces) into a single object, which provides less cluttered spatial tree, reduces the number of texture fetches and speeds up rendering.

**Core Features:**

+**Placement and Removal** - click on empty ground adds a new mesh at the clicked position, click on existing cluster geometry removes the selected mesh instance from the cluster
+**Raycasting and Intersection Testing** - casts a ray from the camera through the mouse position to detect whether the user clicked on a cluster mesh or terrain

**Use Cases:**

+Scattering objects like rocks, grass, or debris
+Dynamic level editing and environment design
+Performance-sensitive applications with many similar mesh instances.