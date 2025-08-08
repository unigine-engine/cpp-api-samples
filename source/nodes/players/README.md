# Players

This sample demonstrates how to create and configure the four available **Players**:

+**PlayerDummy** - a manually controlled camera that has no physical properties and cannot collide with objects.
+**PlayerPersecutor** - a third-person chase camera that follows a target with adjustable distance.
+**PlayerSpectator** - a free-fly camera without a physical body.
+**PlayerActor** - a player with a rigid physical body, which is approximated with a capsule and has physical properties. Can only walk on the ground.

Each player is instantiated with appropriate settings (collision, movement parameters, camera settings), and a GUI panel lets you **switch between them at runtime**. You can also modify camera parameters like **FOV, near/far clipping planes,** etc.