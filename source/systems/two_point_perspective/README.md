# Two-Point Perspective

This sample shows how to simulate a two-point perspective projection using a lens shift technique implemented via a secondary *Dummy Player*. When enabled, the sample dynamically adjusts the projection matrix of a dummy camera to visually align vertical lines while preserving the viewing direction.
The effect is achieved by adjusting the projection matrix of a *PlayerDummy* instance according to the pitch angle of the active camera. During the rendering phase, the *Dummy Player* temporarily replaces the main camera to apply the modified view.

This approach can be used for architectural visualization or stylized camera effects where a more orthographic-like vertical perspective is desired.