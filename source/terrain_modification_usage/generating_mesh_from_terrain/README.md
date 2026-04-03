# Generating Mesh From Terrain

This sample demonstrates how to generate a procedural mesh (**ObjectMeshDynamic**) that represents a selected region of the **Landscape Terrain**.

**Key Features:**

-Terrain data (height and albedo) is fetched at runtime using **LandscapeFetch**
-Mesh resolution, position, scale, and rotation can be customized via UI
-Optionally render a bounding box and wireframe overlay
-Ideal for:

-Runtime terrain previews
-Simplified **LOD** meshes or data extraction for analysis/simulation.

The generated mesh uses a basic material and includes color data sampled from the terrain's albedo layer. Use the **Generate** button to fetch terrain data and construct the mesh in real time.