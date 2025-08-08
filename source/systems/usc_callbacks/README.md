# USC Callbacks

This sample demonstrates how to call *UnigineScript* functions from *C++* code via callbacks.

The mechanism is based on *Variable* Class instances and allows calling both custom and built-in script functions by name.

The sample registers a *C++* wrapper function to expose script invocation capability to *UnigineScript* via the *Interpreter* class.