# Callbacks

This sample demonstrates how to use the *CallbackBase* class via the C++ API to wrap and call functions and class methods with various numbers of arguments.
Callbacks are created using the *MakeCallback* method for both standalone functions and member methods, including parameterized versions with up to four arguments. Once created, callbacks can be executed using *CallbackBase::run()* method, optionally passing different arguments at runtime. This allows storing and triggering functions dynamically through a unified interface.

Callback mechanism is useful in scenarios such as event-driven systems, user interface interactions, or asynchronous task management in applications requiring dynamic function invocation.