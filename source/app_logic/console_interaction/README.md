# Console Interaction

This sample demonstrates how to interact with the Engine's built-in console and add custom console commands and variables via API using the *Console* and *ConsoleVariable* classes. It shows how to define different types of console variables: *ConsoleVariableInt*, *ConsoleVariableFloat*, and *ConsoleVariableString*, and how to register custom console commands.

Commands are linked to callback functions using *MakeCallback*, and can be executed directly from code or entered manually through the console. Commands can also be added and removed dynamically at runtime, making the system flexible for various use cases. Console variables can be accessed or changed through both code and the console interface.

For demonstration, to move the Material Ball in the scene use the custom command `**control_node [x] [y] [z]**` in the Console (`), where *x, y, z* are the target world coordinates (e.g., `**control_node 0 5 1**`).

This functionality can be used for development, debugging, rapid prototyping, and runtime adjustments in interactive applications.