# Cad-Like View

This sample demonstrates how to build a CAD-style layout by combining four synchronized views of the same scene: top, side, front, and perspective. Each view uses its own camera with a separate projection matrix - orthographic for technical views and perspective for the main one. Each view is rendered off-screen and displayed through a *WidgetSpriteViewport*.
The views are arranged in a 2x2 grid using GUI widgets. When the window is resized, texture resolutions are automatically updated to match the new layout and keep the viewports sharp.

Viewcubes are centered on the main object to maintain consistency across all views. They help indicate camera orientation and are updated based on the window size and viewport positions.

This setup is ideal for tools that require accurate inspection of a scene from multiple directions, such as modeling, level design, or CAD-style applications.