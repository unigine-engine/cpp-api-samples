# Event Connection Patterns

This sample demonstrates four different patterns for subscribing UNIGINE's *Events* via the C++ API, highlighting how event handler lifetime and management can vary depending on the approach.

Each method demonstrates a different strategy for connecting to the same event and managing event handler lifetimes:

-*EventConnectionExample* stores a single event handler with manual control over its activation. This type of connection is useful when you need precise control &#8212; you can enable, disable, or fully disconnect the handler at any time.
-*EventConnectionsExample* acts as a container for multiple handlers. It handles cleanup automatically (via the destructor) and manually (by calling *EventConnections::disconnectAll()*). This is useful when you have many event handlers with varying lifetimes that need to be grouped.
-*InheritedEventConnectionExample* inherits *EventConnections* class, making connection management part of its internal logic. All connected handlers are automatically disconnected when the object is destroyed.
-*CallbackIDConnection* provides a low-level, manual way to manage handlers using a connection ID. It offers flexibility but requires careful memory and lifetime handling. This approach is considered unsafe and should only be used when you fully understand the implications.

Each example connects to a shared *EventHolder*, and handlers are triggered with a sample value. This setup is useful when designing modular, reactive systems that rely on flexible and explicit event-driven logic.