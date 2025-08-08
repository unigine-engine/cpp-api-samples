# Landscape Tracks

This sample demonstrates non-destructive runtime modification of the **Landscape Terrain** by dynamically spawning multiple **Landscape Layer Map** beneath moving objects (such as vehicles or characters) to create realistic track imprints on the terrain. Each track is rendered using a dedicated **Landscape Layer Map** that blends additively with the terrain&#8217;s height data and alpha blends with the albedo, producing visually convincing trails without altering the original terrain layers.

**Key Features**

+Automatic track spawning based on object movement
+Track maps are spawned only when the object has moved a minimum distance (user-defined)
+The number of active track maps is limited for performance (configurable limit)
+Albedo and height blending are handled via standard non-destructive layering, ensuring easy track removal or replacement.

**Customizable Parameters**

+**Min Distance Between Tracks** - Controls how frequently new track maps are spawned based on movement
+**Max Number of Track Maps** - Caps the number of simultaneously active track layers to manage memory and performance.

**Use Cases**

+Vehicle or character track systems for games and simulations
+Temporary terrain effects (e.g., tire marks, footprints, sliding trails)
+Runtime procedural content that doesn't permanently modify base terrain.