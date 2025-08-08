# Object Frame

This sample shows how to dynamically create and display rectangular frames around objects in the scene using the *WidgetCanvas*. These frames include object labels and appear only if the object is visible and is located within a certain distance from the camera.

You can also take screenshots via the *Snap Screenshot* button. The captured image does not include the rendered frames, but all frame metadata (including object *ID*, name, transform, and screen coordinates of the frame) is saved to a separate *JSON* file.

The bounding box of each object is computed recursively based on all its mesh components. The resulting rectangle is projected onto the screen space and checked for visibility. If visible and not occluded, a *2D* frame is drawn using *WidgetCanvas* class.

All metadata, such as object *ID*, name, world-space position and rotation, as well as screen-space rectangle coordinates, is collected and saved as a structured *JSON* document using the *Json class*.

This approach is useful for creating visual overlays, annotations, or *UI* debug elements where object-specific data must be highlighted or preserved externally.