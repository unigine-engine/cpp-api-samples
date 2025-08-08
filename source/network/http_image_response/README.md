# Simple HTTP Image Response

This sample demonstrates how to implement an asynchronous *HTTP* server that responds to *REST API* requests by sending image data. It captures the contents of the application window, converts the rendered frame to a *PNG* image in memory, and serves it via an embedded *HTTP* server.

For demonstration, open the *URL* displayed on the plate under the *Material Ball* in the scene.

When a client sends a *GET* request to the **/unigine.png** endpoint, the latest frame is captured, converted, and streamed back as a *PNG* file. The server runs asynchronously, and all resource access is properly synchronized to ensure thread safety.

This sample demonstrates how to expose visual data from a real-time application for use in debugging, remote monitoring, or integration with web-based interfaces.