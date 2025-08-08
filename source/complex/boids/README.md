# Boids

This sample demonstrates the simulation and control of flocking behavior (Boids algorithm) for different types of entities, such as birds and fish. Two separate controllers manage the behavior of these groups using common flocking principles like cohesion, alignment, and separation.

You can adjust various parameters in real time to observe how they influence the behavior of each flock:

+**Cohesion** - strength of attraction toward the center of the flock.
+**Spot Radius** - distance within which other units are considered for cohesion and alignment.
+**Alignment** - force that aligns a unit's direction with the average heading of its neighbors.
+**Separation** - repulsion force that prevents units from crowding too closely.
+**Separation Desired Range** - distance threshold for applying separation force.
+**Target** - amount of force directed towards target.
+**Unit Max Speed** - maximum unit speed.
+**Unit Max Force** - maximum steering force applied for movement corrections.
+**Unit Max Turn Speed** - how quickly a unit can rotate to adjust direction.

Two independent flocks (e.g., fish and birds) demonstrate how multiple controllers can operate simultaneously with distinct settings. Optional debug visualization renders bounding boxes around each flock, helping observe bounds and transitions.

**Use Cases**

+**Games and simulations** - realistic swarm behavior for birds, fish, insects, crowds, or drones.
+**Training AI systems** - testing multi-agent behavior and interaction dynamics.
+**Environmental storytelling** - adding life to ecosystems and natural environments.
+**Cinematic scenes** - choreographed group movements for visual impact.
+**Education and research** - exploring emergent behavior in decentralized systems.