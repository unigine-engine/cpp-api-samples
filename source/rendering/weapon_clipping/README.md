# Weapon Clipping

In first-person games, weapon models can clip through walls or geometry when the camera gets too close. This sample solves the problem by rendering the weapon separately into a texture and compositing it over the main scene.

This approach keeps the weapon always visible, even when the camera is close to walls, without interfering with the main rendering pipeline.

The setup uses two cameras with identical transforms to keep their views aligned. *Viewport masks* for cameras are set via UnigineEditor: the main camera renders everything except the weapon, while the secondary (weapon) camera renders only the weapon. The weapon view is captured into a texture using *Viewport::renderTexture2D()*.

Each frame, this texture is overlaid onto the screen using *Render::renderScreenMaterial()*, compositing the weapon on top of the environment. The component handles screen resizing, maintains isolated render states, and offers optional settings like skipping shadows in the weapon rendering pass.