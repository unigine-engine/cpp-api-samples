# Water Surface Parameters Fetch

This sample demonstrates how various parameters influence the accuracy of **fetch** (sampling water height and normal) and **intersection** (ray-water collision detection) operations on the **Global Water** object across different **Beaufort** levels (the Beaufort slider).

You can interactively adjust the following parameters:

-**Steepness Quality**: Controls wave detail resolution used in sampling.
-**Amplitude Threshold**: Filters out minor waves to improve performance at the cost of detail.
-**Precision**: Controls ray-water intersection accuracy. Lower values reduce jitter when intersecting at an angle.
-**Intersection Angle**: Adjusts the incoming ray direction for intersection tests, helping evaluate how steep angles affect detection stability.

The UI also allows you to:

-Select between **Fetch** and **Intersection** modes
-Show or hide **normals** at the sampled points
-Adjust the **number of samples** and **visual point size**

> [!NOTE]Intersection rays are visualized with blue arrows. If jitter occurs at non-zero intersection angles, try lowering the **Precision** value.
This sample is useful for fine-tuning water interaction accuracy in physics, gameplay, and visual effects - especially when working with sloped or moving viewpoints (e.g., cameras, characters, or objects interacting with water).