#pragma once

#include <UnigineComponentSystem.h>

class StructuredBufferSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(StructuredBufferSample, ComponentBase)

	COMPONENT_INIT(init)

	PROP_PARAM(Material, dxt_compute_material)
	PROP_PARAM(File, source_texture_param, "Source Texture")

private:
	void init();
};
