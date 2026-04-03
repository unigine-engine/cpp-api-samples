#pragma once
#include <UnigineComponentSystem.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Demonstrates frame-rate independent movement using Game::getIFps().
// Shows the difference between fixed-step and time-scaled movement
// to ensure consistent behavior regardless of frame rate.
class IFpsMovementSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(IFpsMovementSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the Game::getIfps and demonstrates"
		"how it affects nodes behaviour.");

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);


private:
	void init();
	void shutdown();

	SampleDescriptionWindow window_sample;
};
