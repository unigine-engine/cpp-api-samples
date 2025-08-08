# Thread

This sample shows how to define and manage background threads in **UNIGINE** by inheriting from the *Thread* class and overriding the *process()* method.

Two custom thread types are demonstrated:

<ol>
+**InfiniteThread** - continuously outputs messages while running.
+**CountedThread** - performs a finite number of iterations before completing.
</ol>

Threads are started during component initialization and executed in parallel with the main engine loop. The infinite thread is explicitly stopped via *stop()* once the counted thread completes all iterations.

This sample illustrates basic principles of multithreading and can serve as a foundation for offloading computations or *I/O* operations from the main thread.