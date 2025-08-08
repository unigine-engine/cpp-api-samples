# Clutter-to-Cluster Converter

This sample demonstrates how to dynamically generate a clutter (**ObjectMeshClutter**) and convert it into an optimized cluster (**ObjectMeshCluster**) in real time. The clutter is generated using a random seed, and the conversion process transfers all key parameters from the clutter: materials, surface properties, LOD, shadows, physics, and collision settings.

The resulting cluster inherits the clutter's transform and hierarchy, but lets you **selectively edit and remove individual elements** of the group. Very often when building your worlds it is necessary to scatter meshes across a certain area randomly and then reposition some of them manually (e.g. when creating forests).

**UI Features:**

+**Generate Clutter** - Creates a randomized layout of clutter meshes.
+**Convert to Cluster** - Merges all clutter instances into a single **ObjectMeshCluster** with identical visual/physical behavior.