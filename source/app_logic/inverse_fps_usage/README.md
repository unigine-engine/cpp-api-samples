# Inverse FPS Usage

This sample demonstrates the importance of using *Game::getIFps()* to implement movement logic independent of the frame rate.

Two cubes move back and forth along the X-axis. Use the *Max render fps* slider to change the frame rate of the application and compare their behavior.

The green cube uses *Game::getIFps()* to scale its movement by the frame time delta, which keeps its speed consistent across varying frame rates.

The red cube does not use *Game::getIFps()* and simply applies constant translation per frame, so its speed changes together with the frame rate.

The movement of both cubes is implemented in the *IFpsMovementController.cpp* file.