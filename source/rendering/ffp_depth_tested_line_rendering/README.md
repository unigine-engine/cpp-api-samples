# FFP Depth-Tested Line Rendering

This sample demonstrates how to render custom visual elements (lines) using the *FFP* with depth testing enabled.

The elements are drawn during the visualizer stage and properly sorted with respect to scene geometry using the depth buffer.

The sample sets up a render callback via *Render::getEventEndVisualizer()*, which is used to draw a simple line segment in camera space. Drawing is performed using *FFP*, with blending and depth-testing configured manually via *RenderState* class.

The scene features a red line intersecting the object. The projection matrix is modified to account for reverse depth and range remapping, ensuring correct visual sorting. The line vertices are transformed into camera space using the current modelview matrix, and rendered in screen space.

This sample can be used for fast rendering of various additional or debug elements (such as semi-transparent objects, frames, *3D* grids and coordinate systems, path traces, motion trails, line-of-sight visualization etc.) while ensuring consistency with the scene content.