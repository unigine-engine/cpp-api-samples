# Landscape Paint

This sample demonstrates several techniques for destructive runtime modification of the **Landscape Terrain** by altering the underlying textures of a **Landscape Layer Map** using **Landscape::asyncTextureDraw()** in combination with custom base materials.

It features three distinct painting modes:

+**Albedo Painter:** Allows painting of the terrain's albedo texture using a customizable brush (texture, mask, size, angle, opacity, and color).
+**Height Painter:** Enables height modification using height values and scaling options, along with support for alpha or additive blending modes.
+**Height To Albedo:** Converts terrain height data into albedo colorization based on a user-selected gradient and specified height range.

You can dynamically change brush parameters through a UI panel (textures, masks, gradient, size, spacing, angle, opacity, and color) and visualize the brush as an orthographic decal projected onto the terrain.

**Use Cases**

+Runtime terrain painting tools for level editors and world-building interfaces
+Terrain sculpting systems in games or simulations
+Custom height-based texturing workflows for stylized or procedurally modified terrains
+Visualization of terrain blending behavior with UI interactivity.