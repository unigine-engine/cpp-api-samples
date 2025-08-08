# Player Persecutor

This sample demonstrates a custom third-person camera built using a *PlayerDummy* node, replicating and extending the behavior of UNIGINE's built-in *PlayerPersecutor* object.

The component calculates camera position and orientation based on user input (if enabled), target movement, and optional collision detection. The anchor point defines the offset relative to the target, while minimum and maximum angles and distances constrain camera movement. If collision is enabled, the camera uses a collision shape to detect and avoid geometry between itself and the target, adjusting its position accordingly. The camera supports both free and fixed rotation modes. When rotation is fixed, the camera maintains a stable angle relative to the target. Otherwise, it can rotate in response to mouse input.

This setup is useful for third-person gameplay, chase cameras, or situations where built-in logic isn't flexible enough and a fully customizable solution is needed.