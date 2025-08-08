# Ray Intersection

This example demonstrates how to find intersections with geometry using **raycasting** taking into account different *Intersection* bitmasks (using first 8 bits out of 32).
Raycasting involves detecting intersections with scene objects along the path of a ray cast from point **A** to point **B**. This process is visualized in the sample via a laser ray representation (laser length depends on the ray length).

In the *update()* method, the *World::getIntersection()* function checks for ray intersections with scene objects based on a specified intersection mask. If the ray hits an object with a matching mask (one bit at least), an intersection is detected. After that, the ray is updated and visual spark effects are rendered at the intersection point.

You can change the laser's intersection mask by toggling the corresponding checkboxes. If a wall's mask bit doesn't match the laser's mask, the laser will pass through.

Using raycasting makes it easy to implement various game mechanics, such as shooting or target detection during navigation.