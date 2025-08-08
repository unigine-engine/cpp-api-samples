# AsyncQueue Tasks

This sample demonstates how to schedule and run different types of tasks using the *AsyncQueue class*. It shows how to execute operations in different thread types, control thread count, and choose whether tasks should complete within the current frame or run freely in the background.

+**Async** - non-blocking execution in a single thread. Useful for offloading tasks without stalling the main thread.
+**Async Multithread** - parallel execution across multiple threads. Each thread receives its own portion of work. Does not block the caller.
+**Frame-Async Multithread** - same as **Async Multithread**, but ensures all threads complete their tasks within the current frame.
+**Sync Multithread** - multi-threaded execution that blocks the calling thread until all threads finish.
+**Frame-Sync Multithread** - same as **Sync Multithread**, but ensures all threads complete their tasks within the current frame.

Refer to the *AsyncQueue* class API for detailed information on available execution modes, thread types, and priorities.