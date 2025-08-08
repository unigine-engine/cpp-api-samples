# Landscape Fetch

This sample demonstrates how to retrieve detailed information from a **LandscapeTerrain object** at any arbitrary point on its surface using **LandscapeFetch**.

**Key Features:**

+Casts a ray from the camera through the mouse cursor
+Performs asynchronous intersection tests with the terrain
+Retrieves:

+Surface **height**
+Surface **normal**
+**Blend mask values** for all configured layers
+Surface albedo

+Visualizes results in the viewport with:

+A **normal vector**
+A floating **text box** showing the terrain fetch data.

This feature can be used for terrain debugging, editor extensions, dynamic content placement, and real-time visualization of terrain attributes.