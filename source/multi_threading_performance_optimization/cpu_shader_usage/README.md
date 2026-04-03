# CPU Shader Usage

This sample demonstrates how to implement a custom CPU shader by inheriting from the *CPUShader* class to perform multi-threaded data processing outside the main rendering loop.

The system updates multiple *ObjectMeshCluster* instances asynchronously by using a helper *AsyncCluster* structure. Each cluster maintains two versions of itself: one for rendering and one for background updates. At the end of each frame, the two are swapped so the visible cluster always shows the latest result without stalling the frame.

This approach is particularly effective for real-time procedural animation, large-scale mesh updates, or any CPU-side logic that benefits from multithreading while remaining synchronized with rendering.

The parallel logic is encapsulated in a derived shader class *UpdateClusterCPUShader*, where the *process()* method is overridden to perform per-cluster updates. This method is automatically dispatched using *CPUShader::runAsync()*, allowing the workload to be spread across available CPU threads. Swap operations, visibility checks, and update decisions are handled independently for each cluster.