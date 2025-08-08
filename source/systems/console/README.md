# Console

This sample demonstrates how to interact with the engine's built-in console and add custom console commands and variables via API using the *Console* and *ConsoleVariable* classes.
It shows how to define different types of console variables: *ConsoleVariableInt*, *ConsoleVariableFloat*, and *ConsoleVariableString*, and how to register custom console commands. Commands are linked to callback functions using *MakeCallback*, and can be executed directly from code or entered manually through the console.

Commands can also be added and removed dynamically at runtime, making the system flexible for various use cases. Console variables can be accessed or changed through both code and the console interface.

This can be used for development, debugging, rapid prototyping, and runtime adjustments in interactive applications.