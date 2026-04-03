# Real-Time Excavation

This sample demonstrates **destructive, real-time modification** of a **Landscape Layer Map** using a 3D object (e.g., a tractor grader) as an excavation tool.

**Key features:**

-Samples base terrain height from a **source layer**
-Projects the grader's **depth map** onto the terrain using an **orthographic camera**
-Uses a custom material (**digging_tool.basemat**) to update height data and clear mask layers
-Applies updates asynchronously using *Landscape::asyncTextureDraw()*
-Supports full interactive movement and rotation of the grader with the *Manipulator* widget.

This sample is ideal for developing terrain deformation mechanics in simulators, construction games, or any scenario requiring runtime landscape sculpting based on object interaction.