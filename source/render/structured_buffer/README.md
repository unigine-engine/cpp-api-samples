# Structured Buffer

This sample demonstrates how to compress a texture into **DXT1** format using compute shaders and structured buffers.

At initialization, a compute material is loaded, and a source texture is prepared. A *DXT1Block* structure is used to hold the compressed data, with its size based on the texture's dimensions (in 4x4 blocks). A structured buffer is created on the GPU to store the output.

The compute shader runs in two stages: a warm-up to trigger shader compilation and the actual compression pass. The number of compute groups is calculated dynamically from the texture size and thread group size to achieve efficient parallel execution.

When compression is complete, the GPU buffer is transferred to CPU memory asynchronously. A new image with the compressed data is created in **DXT1** format and saved to a `*.dds` file.

This method offloads all heavy processing to the GPU, minimizes CPU overhead, and takes advantage of structured memory access for optimal performance.