# Bones Simple Foot Placement

This sample demonstrates a naive option for placing feet on a surface using IK chains.
Raycasting is applied to detect surface contact: in each frame, a ray is cast downward from the foot position to find intersections with the surface. The resulting contact point and surface normal are then used to adjust the foot bones' position and orientation, ensuring realistic foot placement.