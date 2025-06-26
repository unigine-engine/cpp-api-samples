#pragma once

#include <UnigineComponentSystem.h>

class ComputeShaderImage : public Unigine::ComponentBase {
	COMPONENT_DEFINE(ComputeShaderImage, ComponentBase)

	PROP_PARAM(Material, compute_material)
	PROP_PARAM(Material, target_material)

	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::TexturePtr dynamic_texture;
	Unigine::RenderTargetPtr render_target;
};
