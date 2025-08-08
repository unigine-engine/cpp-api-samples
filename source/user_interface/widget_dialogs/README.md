# Dialog Widget

This sample demonstrates how to create and manage a GUI dialog system using UNIGINE's dialog widgets via the C++ API. A *WidgetWindow* instance contains several buttons, each opening a different type of dialog: *WidgetDialogMessage*, *WidgetDialogFile*, *WidgetDialogColor*, or *WidgetDialogImage*.
Each button is connected to an event handler that creates and configures the corresponding dialog widget. Dialogs are shown centered on screen and support user interaction through *OK* and *Cancel* buttons. A shared handler manages these responses, logs actions to the console, and deletes the dialog afterward to clean up resources.

This approach can be used to build simple and interactive GUI tools, settings panels, or development utilities that require built-in dialog interactions.