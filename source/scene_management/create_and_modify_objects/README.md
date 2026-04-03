# Create And Modify Objects

This sample demonstrates the basics of working with ***Objects*** - how to create and modify objects via API, highlighting the differences between various types of objects (*ObjectMeshStatic, ObjectMeshDynamic, ObjectParticles*). **Objects** are characterized by a set of surfaces, an optional physical body and type-specific properties.

In this sample, we create and configure three types of objects with interactive controls:

**1. Static Mesh Object (Material Ball)**

-Load an existing **.mesh** file *(material_ball.mesh)*.
-Assigns a material to the object.
-Adds a *Body* with a *Box-shaped* collision.

**2. Dynamic Mesh Object (Tetrahedron)**

-Constructs a custom *ObjectMeshDynamic* manually.
-Applies a glass-like material.
-Adds a *Body* with a *Sphere-shaped* collision.

**3. Particle System**

-Creates a default *ObjectParticles* object.
-Configures *particle-specific parameters*, such as spawn rate and lifetime, and enables a *collision mask* for particle interaction.

**Runtime Modifications (via Sample Menu)**

-Adjust the albedo color of the **Static Mesh** material.
-Modify the mesh (tetrahedron base height) in the **Dynamic Mesh**.
-Tune the spawn rate of the **Particle System**.
-Toggle the visualization of surfaces and physical shapes of the objects.

For more complex objects, such as *ObjectWaterGlobal* and *ObjectLandscapeTerrain*, see the corresponding samples.