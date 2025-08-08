# Simple HTTP Response

This example shows how to implement a simple asynchronous *HTTP* server that returns current world coordinates of the player in response to external *REST API* requests.

For demonstration, open the *URL* displayed on the plate in the scene.

The sample uses a lightweight embedded *HTTP* server to handle incoming *GET* requests to the **/player_pos** endpoint. When accessed, the server responds with the current world-space position of the active player as plain text.

The server runs directly inside the simulation and starts automatically with the sample. The *URL* is dynamically generated based on the current *IP* and port settings.

This sample can serve as a foundation for live telemetry, debugging, integration with third-party monitoring systems, or any use case where external tools need access to runtime data from the simulation environment.