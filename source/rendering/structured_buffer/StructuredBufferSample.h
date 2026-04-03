// Demonstrates GPU structured buffer usage for DXT texture compression.
// Uses a compute shader to compress a source texture into DXT format
// via a structured buffer, showcasing GPU-side data processing.

#pragma once

#include <UnigineComponentSystem.h>

// Compresses a texture to DXT1 format using GPU compute shader and structured buffer.
class StructuredBufferSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(StructuredBufferSample, ComponentBase)

	COMPONENT_INIT(init)

	// Material with compute shader for DXT compression
	PROP_PARAM(Material, dxt_compute_material)
	// Source texture to be compressed
	PROP_PARAM(File, source_texture_param, "Source Texture")

private:
	void init();
};
