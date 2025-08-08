# Multiple Async Requests

This sample demonstrates how to launch and manage a large number of asynchronous ray-based intersection queries simultaneously.

The results are visualized in real time and latency statistics are displayed for performance analysis.

In this sample, an emitter object continuously rotates and moves vertically, casting rays in multiple directions (slices and stacks) to detect intersections with objects in the world. Each ray is handled asynchronously.

All active requests are monitored for completion. Once finished, the results (intersection points and normals) are visualized using *Visualizer* tools. A latency histogram is computed based on how many frames passed between the request and response, and the results are displayed in the *UI*.

The sample uses double-buffering for safe multi-threaded access and demonstrates efficient scheduling of a high number of asynchronous operations. This approach is useful for stress-testing intersection systems, profiling async request latency, or building interactive tools relying on high-frequency spatial queries.