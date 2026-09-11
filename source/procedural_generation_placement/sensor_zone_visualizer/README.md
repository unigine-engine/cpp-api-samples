# Sensor Zone Visualizer

This sample demonstrates procedural generation of frustum meshes for visualizing sensor fields of view. Four frustums are placed on a single object, facing *forward*, *back*, *left* and *right*. Each frustum is built as a spherical sector (not a real frustum) with adjustable parameters:

-**Near/Far distance** - radial distances of the spherical caps
-**Left/Right frustum border** - horizontal angle range
-**Lower/Upper frustum border** - vertical angle range
-**Color** - color of the frustum mesh.

Move the sliders to rebuild the selected frustum's mesh dynamically.

Fields of view greater than 180 degrees are supported, so the frustums can visualize sensors ranging from a narrow-angle camera to a hemispherical fisheye.

The mesh generation lives in *SensorZoneVisualizer.cpp*; the tabbed parameter panel is built in *SensorZoneVisualizerSample.cpp*.