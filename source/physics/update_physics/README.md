# Update Physics

This sample demonstrates the difference between *update()* and *updatePhysics()* methods.
The sample features two physics-enabled cubes that move back and forth along the X-axis. The movement logic is implemented via in the *UpdatePhysicsUsageController.cpp* file. Use the **Max FPS** slider to change the target frame rate.

The green cube uses *updatePhysics()*, which is called at a fixed physics frame rate, and should be considered a correct example for physics-related logic.

The red cube uses *update()*, which runs every render frame. This approach may cause unstable results when interacting with physics and should generally be avoided when implementing physics-driven movement.

Use *updatePhysics()* to implement continuous or physics-dependent operations (e.g., force application, collision response), as it runs at a fixed time step, unlike *update()* which depends on the rendering frame rate.