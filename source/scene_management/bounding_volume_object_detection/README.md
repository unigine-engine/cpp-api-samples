# Bounding Volume Object Detection

This sample demonstrates how to detect intersections between a frustum, sphere, or box and the bounding boxes of nodes. For clarity, the boundaries of the nodes are displayed in the scene, allowing for a quick visual assessment of when the nodes intersect the specified volumes.

This approach can be used to implement systems for determining which objects fall within a bounding volume and triggering events, such as starting dialogues, spawning enemies, activating effects, or any other reaction.

The *init()* function creates three types of bounding volumes: a frustum, a sphere, and a box. The *update()* function performs the intersection checks. It uses the *World::getIntersection()* function to get a list of nodes that intersect the given boundaries and changes their color using the *change_color()* function.