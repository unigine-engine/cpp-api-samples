# LiDAR

This sample demonstrates a realistic **LiDAR** sensor simulation, used in self-driving cars, robotics, robot vacuum cleaners, and drones to map their surroundings. It works by combining four (or optionally more) virtual depth cameras to create a full **360**-degree scan. The **LiDAR** emits rays and measures distances by rendering a depth map of the environment. In addition, a response intensity for each ray is calculated using normal, roughness and metalness from the G-buffer. You can tweak its settings (scan range, FOV, resolution, etc.) via API.

**Key Features:**

+Emulated LiDAR using **4 or more** rendered views
+Configurable **min/max range, FOV, beam resolution** (number of stacks and slices)
+Asynchronous data transfer using *asyncTransferTextureToImage*
+Dynamic beam caching, image post-processing, and world-space point rendering
+Optional visual debugging: depth maps, scan points, and frustums
+Auto-refreshing system with internal transform and scan updates.

**Use Cases:**

+Autonomous vehicle simulation (robot vacuums, drones, cars)
+Autopilot and AI training using virtual LiDAR input
+Robot navigation and localization (SLAM, path planning).