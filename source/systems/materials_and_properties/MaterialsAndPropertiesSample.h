#pragma once

#include <UnigineComponentSystem.h>

class MaterialsAndPropertiesSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(MaterialsAndPropertiesSample, ComponentBase)

	COMPONENT_INIT(sample)
	COMPONENT_SHUTDOWN(shutdown)

private:
	void sample();
	void shutdown();

private:
	bool is_console_onscreen{false};
};
