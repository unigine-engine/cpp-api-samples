# AsyncQueue

This sample shows how to load resources like meshes and textures in the background using the *AsyncQueue* class. Files are loaded in a separate thread, so the main application stays responsive.

Meshes and textures are added to the loading queue, and the system listens for events to know when each resource is ready. When a mesh finishes loading, it's removed from the queue. For textures, an event handler is used to handle their completion. The sample also demonstrates how to group and manage resource requests, making it easier to control the loading process.

This kind of async loading is useful for streaming large levels, loading assets on demand in VR, or preloading data in simulations without freezing the interface.