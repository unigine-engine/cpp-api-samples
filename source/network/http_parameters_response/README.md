# HTTP REST Request

This example demonstrates how to implement an asynchronous HTTP server that receives player position via external REST API requests.

For demonstration, open the *URL* displayed on the plate in the scene with x and y coordinates in the request parameters .

The sample uses a lightweight embedded HTTP server to handle incoming POST requests. When accessed, the server reads the x and y parameters from the request and updates the world-space position of the active player accordingly (the z coordinate remains unchanged).

The server runs directly inside the simulation and starts automatically with the sample. The URL is dynamically generated based on the current IP and port settings.

This sample can serve as a foundation for live telemetry, debugging, integration with third-party monitoring systems, or any use case where external tools need access to runtime data from the simulation environment.