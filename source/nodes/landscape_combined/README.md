# Landscape Combined Modification

This sample demonstrates a hybrid approach to terrain editing using both **non-destructive** (using multiple **Landscape Layer Maps**) and **destructive** (using **Landscape::asyncTextureDraw()** method) **Landscape Terrain** modification techniques.

**It works by:**

+Sampling height data from a **Source** Landscape Layer Map
+
Using **Landscape::asyncTextureDraw()** in combination with a custom material additive height blending to modify the **Target** Layer Map if the sampled height is above the provided threshold:

+The target terrain is flattened (clamped to the height threshold)
+The target albedo (on the threshold height level) is painted orange.

This sample is ideal for tools that require selective terrain stamping, erosion masks, or custom terrain sculpting.