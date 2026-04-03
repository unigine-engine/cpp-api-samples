# First-Person Controller

This sample demonstrates a first-person character controller implemented as a component attached to a *PlayerDummy*.

The controller uses Shape-Surface collisions to detect ground, walls, and slopes, and applies slope-aware movement to ensure stable walking on inclined geometry. It supports walking, running, jumping, air movement, smooth crouching, and camera rotation with vertical limits. Auto-stepping allows the character to traverse small obstacles, and Shape-Shape collisions are used to interact with physical objects by applying impulses.