# Weapon Clipping

In first-person games, weapon models can clip through walls or geometry when the camera gets too close. This sample solves the problem by rendering the weapon separately into a texture and compositing it over the main scene.

Two cameras with identical transforms are used to ensure alignment. Their Viewport masks are set via UNIGINE Editor in the following way: one renders the environment without the weapon, and the other renders only the weapon. The weapon view is drawn into a texture using *Viewport::renderTexture2D()*, and then overlaid onto the screen with *Render::renderScreenMaterial()*. The system handles window resizing, render-state isolation, and allows optional settings like skipping shadows in the weapon rendering pass.

This approach keeps the weapon always visible, even when the camera is close to walls, without interfering with the main rendering pipeline.