# Bones: Sandbox

This sample provides the interface that allows visualizing and experiencing how to configure all available settings for IK chains, LookAt chains, and bone rotation constraints.

The skeleton of the character is displayed as a tree, where you can pick the bone to work with and see it highlighted in the viewport. Three editors are available for it:

-**IK** - creating chains and adjusting the target and pole positions, the rotation of the effector, the number of solver iterations, the tolerance, and the constraint modes.
-**LookAt** - creating chains, adding bones to them, setting the target and pole positions, and adjusting the weight and the axes of each bone.
-**Constraints** - setting the minimum and maximum angles of rotation around the yaw, pitch, and roll axes for a certain bone.

> [!NOTE]Visualization of the bones requires a *Debug* or *Development* build, it is not available in the *Release* one.