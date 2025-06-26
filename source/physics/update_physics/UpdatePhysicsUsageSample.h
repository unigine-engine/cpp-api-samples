#pragma once
#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class UpdatePhysicsUsageSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(UpdatePhysicsUsageSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the update/physics_update and demonstrates"
		"how it affects nodes behaviour.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);


private:
	void init();
	void shutdown();

	SampleDescriptionWindow window_sample;
};
