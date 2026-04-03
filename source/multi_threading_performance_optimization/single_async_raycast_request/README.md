# Single Async Raycast Request

This sample demonstrates how to perform a single asynchronous intersection query based on the user's mouse cursor position in the scene. The result includes the hit point and surface normal, which are visualized in the scene, along with latency information.

This setup demonstrates how to implement non-blocking intersection queries suitable for object selection or similar real-time input-driven interactions.

The sample demonstrates detection of intersections with all objects in the world using a combination of *World::getIntersection()* and *Landscape::getIntersection()* methods. A single ray is cast from the current camera position through the mouse cursor. If the ray intersects with any geometry, the hit point and surface normal are rendered using *Visualizer*. Intersection queries are handled asynchronously. A callback processes the result and records latency in frames. The average and maximum latency values are updated in real time and shown in the sample *UI*.