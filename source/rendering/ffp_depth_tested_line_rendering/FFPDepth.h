// Demonstrates FFP (Fixed-Function Pipeline) rendering with depth testing.
// Renders 3D lines that properly interact with scene depth, unlike
// standard FFP which renders on top of everything.

#pragma once

#include <UnigineComponentSystem.h>

// Renders depth-tested FFP lines via end-visualizer callback.
class FFPDepth : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(FFPDepth, ComponentBase)
	COMPONENT_INIT(init)

private:
	void init();
	void render_lines_callback();		// Draws lines with depth test enabled
};
