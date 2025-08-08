# WidgetExtern

This sample demonstrates how to create a custom GUI widget by inheriting from *WidgetExternBase* via the C++ API. The custom class overrides key methods like *render()*, *arrange()*, and *checkCallbacks()* to implement unique visual and interactive behavior.
The widget is registered with a unique class ID using *WidgetExternBase::addClassID()*, and can then be instantiated using *WidgetExtern::create()*.

This setup allows full control over widget rendering and logic directly in C++.