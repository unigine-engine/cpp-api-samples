# Widget Window

This sample demonstrates how to create a basic *WidgetWindow* using the C++ API and handle user interactions (edit line and button press events) through handler function connections.

The window includes a *WidgetEditLine* and a *WidgetButton*. The *WidgetEditLine* listens for text changes via *Widget::getEventChanged()* and logs the new input, while the WidgetButton uses *Widget::getEventClicked()* to trigger an action when pressed. Font sizes are adjusted for better readability, and the layout is arranged automatically.

This approach is suitable for building interactive UI elements directly within UNIGINE's native GUI system.