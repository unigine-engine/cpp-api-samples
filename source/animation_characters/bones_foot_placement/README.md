# Bones: Foot Placement [Animation Graph]

This sample demonstrates a naive option for placing feet on a surface using inverse kinematics in an *animation graph*: a *Two Bone IK* node per leg bends them towards the points the feet have to reach, with the pelvis position adjusted so that both legs can reach the surface.

Raycasting is applied to detect surface contact: in each frame, a ray is cast downward through the foot position to find intersections with the surface. The resulting contact point and surface normal are then used to adjust the foot bones' position and orientation, ensuring realistic foot placement. Both the rays and the normals found at the contact points are visualized on the character.

Each leg blends into inverse kinematics only while its ray hits the surface, so a foot that steps off the platform returns to the animated pose smoothly instead of snapping.

The platform under the character can be moved with the manipulator switched on by the *T* key and rotated with the one switched on by the *R* key, so you can watch the feet adapt to the changing height and slope in real time.