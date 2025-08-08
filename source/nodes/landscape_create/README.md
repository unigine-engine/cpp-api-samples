# Landscape Creation

This sample demonstrates how to dynamically generate a **Landscape Layer Map** using tiled **albedo, height,** and **mask** textures. The process uses the **LandscapeMapFileCreator** and **LandscapeMapFileSettings** APIs to build a fully functional **.lmap** file and configure it for use with the **Landscape Terrain** object.

**Key features:**

+Supports tile-based input with user-defined grid size and resolution
+
Loads and validates image tiles for:

+**Albedo**
+**Height**
+**Grass mask**
+**Stone mask**

+Applies blend settings for each layer using ***LandscapeMapFileSettings***
+Automatically creates and attaches the landscape objects to the world.

This workflow is ideal for runtime terrain generation, procedural terrain tools, or editor extensions that need to assemble terrain data from image sources.