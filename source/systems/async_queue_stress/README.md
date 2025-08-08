# AsyncQueue Stress

This sample demonstrates how to asynchronously load large number of nodes using the *AsyncQueue* class while ensuring correct activation on the main thread.

In UNIGINE, world nodes must be created only from the main thread. To comply with this restriction and avoid blocking the main thread, the sample performs the initial node loading in a background thread, and then schedules a follow-up task on the main thread to finalize activation by calling *updateEnabled()* - a method that registers the node and its children in the world's spatial structure.

With the built-in Profiler enabled, you can observe how the engine handles increasing load smoothly and avoids frame spikes.

Refer to the *AsyncQueue* class API for detailed information on available execution modes, thread types, and priorities.