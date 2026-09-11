# Measurements

This sample shows how to measure distances in a scene at runtime and display them as on-screen rulers.

A set of target objects is placed around a single movable node. Every frame the distance from this node to each target is measured and drawn with the *Visualizer* as an arrow with a 3D label showing the length in meters. The arrows are clipped to object surfaces for precise surface-to-surface measurements. 

The ruler color transitions from red (near) to green (far), and a white zone circle is drawn around each target. Green distance threshold and zone radius are adjustable in the **Parameters** window.

The reusable parts are split into standalone components:

-**DistanceMeasurement.cpp** - measures distances and draws rulers to each target
-**ZoneRadiusVisualizer.cpp** - draws the zone circle around a node
-**DistanceVisualizer.cpp**- provides shared drawing logic
-**SimpleMovement.cpp (`Node Movement` sample)** - moves the controlled node