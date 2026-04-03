# Spline Graph

This sample demonstrates how to create and visualize a spline graph (***SplineGraph***), and move an object along it. A **SplineGraph** is created at *Initialization* by adding points and segments using the *addPoint(), addSegment()* API methods, with control points placed randomly.

-A cube mesh is created and added to the scene.
-During the *Update* stage, the cube moves along the spline by switching between its consecutive segments and evaluating a point on it using the *calcSegment** methods.