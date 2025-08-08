# Buoyancy

This sample demonstrates realistic water interaction in UNIGINE, showcasing both the current state of **Global Water** control via changing **Beaufort** levels and the use of fetching the water level at a certain point for a simplified simulation of buoyancy without engaging Physics.

**Core Features:**

**Global Water Control.** Adjust ocean conditions in real-time using the **Beaufort slider** **(0-12)**. Higher values create stormier waves with enhanced foam and detail.

**Optimized Buoyancy System**. Simulates floating objects without full physics collisions. Uses three anchor points for stable wave-following behavior. It then smoothly adjusts the object's position and rotation to match the waves, with calculations based on:

+Object mass (customizable)
+Global buoyancy coefficient (adjustable via **Buoyancy slider**)
+Water surface steepness and wave height.

**Use Cases:**

+Marine simulations
+Weather system prototyping
+Performance-sensitive scenes with many floating objects
+Games needing stylized water interactions.