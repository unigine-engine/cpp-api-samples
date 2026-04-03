// Fixed Function Pipeline (FFP) rendering demonstration. Shows how to use
// immediate-mode triangle rendering for simple 2D overlays without shaders.

#pragma once

#include <UnigineComponentSystem.h>

// Renders a rotating colored fan using FFP immediate-mode drawing.
// Hooks into the post-GUI render event to draw after UI is complete.
class FFPSample : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(FFPSample, ComponentBase)
	COMPONENT_DESCRIPTION("This component demonstrates using the Fixed-Function Pipeline (FFP) to render "
							"dynamic geometry with per-vertex colors after GUI rendering.")

	COMPONENT_INIT(init)

private:
	void init();

	// Custom FFP render callback, executed after the engine finishes GUI rendering
	static void render();
};
