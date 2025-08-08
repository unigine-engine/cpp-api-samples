# Physical Buoyancy

This sample demonstrates how to implement **physically accurate buoyancy** for dynamic objects floating on **Global Water**. A physical body is divided into a grid of virtual volume elements, each sampled independently for water height. Based on how much of each cell is submerged, the system:

+Calculates and applies distributed **buoyant forces**
+Adds appropriate **torque** to simulate rotation
+Applies **linear and angular damping** depending on submerged volume.

You can also define a **custom center of mass**, and optionally enable **debug visualization** of submerged sections, force directions, and sampling points via API. Use the **Global Water** object across different **Beaufort** slider parameter to adjust wave intensity.

This approach is useful for games and simulators involving boats, ships, debris, or physics-based water interactions.