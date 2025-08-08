# Canvas Widget

This sample demonstrates how to use the *WidgetCanvas* class to draw vector-based shapes and text. The canvas supports adding lines, polygons, and text by defining their geometry through vertex positions. Elements are layered by draw order and colored individually.
The canvas content remains sharp regardless of scaling or rotation. In this example, the entire canvas is transformed over time, rotating and moving in 3D space while preserving the clarity of its contents. The geometry is built using helper functions that generate radial lines and regular polygons, and the layout is updated every frame using a transformation matrix.

This can be used to create clean, scalable UI elements, procedural charts, custom overlays, or dynamic data visualizations in tools or applications.