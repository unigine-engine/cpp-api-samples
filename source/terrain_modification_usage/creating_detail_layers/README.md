# Creating Detail Layers

This sample demonstrates how to add and manage detail layers for a **LandscapeTerrain** object using the *getDetailMask()* and *addDetail()* methods.

It dynamically creates and configures three types of terrain details:

-**Grass** 
-**Stone**
-**Snow caps** (simple white material applied based on elevation using height constraints - no texture).

**Each detail is:**

-Mapped to a separate detail mask
-Assigned a custom material
-Ordered visually using *swapRenderOrder()* to ensure natural blending (stone below grass, grass below snow).

This setup is useful for biome-style terrain detailing, visual layering of materials, and real-time terrain customization.