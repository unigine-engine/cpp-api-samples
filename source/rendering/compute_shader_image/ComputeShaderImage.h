// Demonstrates compute shader usage for dynamic texture generation.
// Runs a compute shader that writes directly to a texture using
// unordered access, which is then displayed on a target material.

#pragma once

#include <UnigineComponentSystem.h>

// Generates procedural texture via compute shader and displays it on material.
class ComputeShaderImage : public Unigine::ComponentBase {
	COMPONENT_DEFINE(ComputeShaderImage, ComponentBase)

	// Material containing the compute shader
	PROP_PARAM(Material, compute_material)
	// Material that displays the generated texture
	PROP_PARAM(Material, target_material)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void update();
	void shutdown();

private:
	// Texture written to by compute shader
	Unigine::TexturePtr dynamic_texture;
	// Render target for compute dispatch
	Unigine::RenderTargetPtr render_target;
};
