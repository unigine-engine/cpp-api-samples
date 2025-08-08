# StreamBase

This sample demonstrates how to create a custom stream class by inheriting from *StreamBase* and use it for reading from and writing to files. The resulting stream is used to serialize and deserialize basic data types to and from a binary file.
The sample provides a wrapper around standard *C* file *I/O* functions and integrates with the **UNIGINE** stream system by implementing the *StreamBase* interface. In the sample logic, a binary file is first created and filled with data via *Stream::writeString()*, *writeInt()*, and *writeFloat()*. Then the file is reopened in read mode and the same values are read back using the corresponding *Stream 'read'* methods, verifying the functionality of the custom stream.

This example serves as a reference for implementing custom stream sources (e.g., from memory, network, or virtual filesystems) and integrating them with the engine's serialization tools.