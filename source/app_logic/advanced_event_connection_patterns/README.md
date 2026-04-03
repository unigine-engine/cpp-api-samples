# Advanced Event Connection Patterns

This sample demonstrates advanced usage of the UNIGINE **event system**.

*EventsAdvancedSample.cpp* triggers custom rotation events when specific keys are pressed. Each event passes one or more arguments to connected listeners.

*EventsAdvancedUnit.cpp* shows how to connect various types of handlers, including:

-Class methods with extra arguments
-Free functions with discarded or additional arguments
-Lambdas using *connectUnsafe()*
-Storing connections using *EventConnection* or *EventConnectionId* for later disconnection

This sample helps understand flexible patterns for event handling in modular component systems.