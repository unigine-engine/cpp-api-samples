# FFP

This sample demonstrates how to render a simple 2D shape using the *Fixed Function Pipeline (Ffp)* in UNIGINE via the C++ API. A colorful figure composed of 16 vertices is drawn directly on screen using orthographic projection.
The rendering logic is called every frame by hooking into the engine's render loop. Ffp mode is enabled for drawing and then disabled, keeping this rendering isolated from the rest of the frame.

This approach is useful for quick visualization overlays, editor tools, or debugging tasks where shader-based rendering isn't required.