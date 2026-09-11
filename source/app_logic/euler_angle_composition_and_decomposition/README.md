# Euler Angle Composition And Decomposition

This sample demonstrates how the order in which Euler angles are applied changes the resulting rotation. The same three angles applied as *XYZ, XZY, YXZ, YZX, ZXY*, or *ZYX* produce six different orientations.

The sample also does the opposite: it takes the current rotation of the object and converts it back into **Pitch, Roll**, and **Yaw** angles for the selected sequence, displaying them as you rotate the object. These angles may differ from the ones used to set the rotation because of gimbal lock.

The local axes of the object, the global axes, and a gimbal ring for each rotation step are drawn with the *Visualizer*, so every stage of the sequence is visible.

> [!NOTE]Visualization of the bones requires a *Debug* or *Development* build, it is not available in the *Release* one.