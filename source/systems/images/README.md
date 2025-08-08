# Images

This sample shows how to procedurally generate *3D* image data and use it as a density texture for a volume-based material in real time.

The result is visualized using a volume box that updates dynamically every frame based on custom field simulation.

The sample initializes a *3D* image with a predefined resolution and fills it with voxel data derived from a set of moving fields. Each field represents an abstract spherical influence zone that contributes to the voxel density based on distance. Field dynamics are updated every frame to simulate motion.

The raw image data is accessed directly via a pixel pointer and modified per frame, with density values mapped to *RGBA* channels. The resulting image is then uploaded to a material as a *3D* texture used in a volumetric shading model.

The sample demonstrates how to work with the *Image* class and update *GPU* resources efficiently at runtime. This approach can be used for generating volumetric effects such as clouds, fog, or procedurally driven visual phenomena.