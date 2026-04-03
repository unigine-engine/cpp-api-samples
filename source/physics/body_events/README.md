# Body Events

This sample demonstrates how to use the *Frozen*, *Position*, and *ContactEnter* events of the *Body* class via the C++ API.

These events allow detecting when a body comes to rest, moves, or collides with another body or surface.

The sample builds a pyramid of boxes by cloning a mesh and arranging it in several layers. Physics settings are adjusted to improve the stability of the stacked boxes and ensure accurate detection of movement or rest states.

This approach is useful for debugging physical behaviors, providing visual feedback in simulations, or triggering interactive logic based on changing body states.

*Frozen*, *Position*, and *ContactEnter* events of each body are connected with the corresponding handlers. All subscriptions are managed through an *EventConnections* class instance, which keeps all connections in one place and ensures proper cleanup during shutdown. For example, the *Frozen* handler changes the material of the box when it stops moving. The *Position* handler changes the material whenever the position updates. The *ContactEnter* handler visualizes contact points during collisions.