# Triggers

This sample illustrates how to implement a simple trigger component in five different ways:

+**Intersection Trigger** performs the bound check via *World::getIntersection()*.
+**Math Trigger** performs the check if the object is inside the trigger sphere or cube using the object coordinates (the simplest and most performance-friendly way).
+**World Trigger** uses the built-in *WorldTrigger* node, which automatically detects when nodes enter or leave a predefined volume.
+**Physical Trigger** uses the built-in *PhysicalTrigger* node, triggers events when a physical object gets inside or outside it. To be detected by the trigger, physical objects are required to have at the same time both a body and a shape.
+**Node Trigger** uses the built-in *NodeTrigger* node, which has no visual representation and triggers events when like being enabled or moved.