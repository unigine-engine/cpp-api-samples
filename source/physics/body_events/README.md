# Body Events

This sample demonstrates how to use the *Frozen*, *Position*, and *ContactEnter* events of the *Body* class via the C++ API. These events allow responding to physical state changes, such as when a rigid body comes to rest, moves, or collides with another object or surface.

The sample builds a pyramid of boxes by cloning a mesh and arranging it in several layers. Physics settings are adjusted to improve the stability of the stacked boxes and ensure accurate detection of movement or rest states.

This approach is useful for debugging physical behaviors, providing visual feedback in simulations, or triggering logic based on changing body states.

Each body in the pyramid is connected to *Frozen*, *Position*, and *ContactEnter* events using the corresponding handlers. All subscriptions are managed through an *EventConnections* class instance, which keeps all connections in one place and ensures proper cleanup during shutdown.

For example, the *Frozen* handler changes the material of the box when it stops moving. The *Position* handler changes the material whenever the position updates. The *ContactEnter* handler visualizes contact points during collisions.