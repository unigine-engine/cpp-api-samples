# Crane Rope

Rope physics can be extremely difficult. The sample features a simple and elegant way of creating a winch or hoist using a combination of dynamically added *JointBall* and geometry.
You can rotate the crane, adjust the rope length, change the mass of the attached load, and detach the load to observe the resulting motion. Enabling the *Visualizer* via checkbox in the sample's UI allows you to see how the position of the joint ball changes in response to these parameter adjustments. The rope also includes an optional tension compensation feature, which helps maintain rope stability and realism under varying loads.

This type of winch is suitable for simulation of helicopter operations or heavy duty equipment towing. The rope is implemented as a C++ Component that you can use in your project.