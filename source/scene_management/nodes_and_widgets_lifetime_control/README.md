# Nodes And Widgets Lifetime Control

This sample demonstrates how node and widget lifetime settings affect object persistence during world transitions.

At startup, the sample creates a plane node and a screen button, each with configurable lifetime modes. Switch the widget between **WORLD** and **WINDOW** lifetime modes, and the node between **WORLD** and **ENGINE** modes.

Use the `**world_load world_name**` console command (e.g., `**world_load arcade**`) to switch worlds and observe how different lifetime settings affect object survival. *WORLD-lifetime* objects exist only in the current world, while *ENGINE-lifetime* nodes and *WINDOW-lifetime* widgets remain active after loading a different world.