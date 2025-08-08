# Gui to Texture

This sample demonstrates how to render GUI elements into a texture using *Gui::render()*. Instead of drawing directly to the screen, the GUI is redirected to a custom framebuffer, which isolates its rendering pipeline and allows the resulting texture to be applied to materials.
The **GuiToTexture** component supports two update modes:

+In **Manual** mode, the texture is updated only when explicitly calling *renderToTexture()*. This is used in the **WidgetClock** example, where the GUI (a digital clock) is re-rendered once per second, only when the displayed time changes.
+**Automatic** mode is enabled by default and updates the GUI texture every frame. This is demonstrated in the **WidgetNoSignal** example, where a *"No Signal"* label moves across the screen like a screensaver. Because the position of the widget changes every frame, the texture must be continuously updated to reflect those changes.

The render flow involves saving and clearing the current render state, binding a texture, configuring the viewport, rendering the GUI widgets, and restoring the render state. Mipmaps are also generated to ensure proper filtering at different scales and distances.

You can use this sample to display dynamic GUI elements on in-game monitors, control panels, or other similar surfaces.