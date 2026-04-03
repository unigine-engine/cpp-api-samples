#pragma once

#include <UnigineComponentSystem.h>

// Demonstrates ray-casting from screen coordinates to world space.
// Converts mouse position to a direction vector via getDirectionFromMainWindow,
// then performs World::getIntersection along that ray. The intersection mask
// parameter filters which objects are tested against.
class IntersectionMouseRay : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(IntersectionMouseRay, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Float, distance, 100.0f);
	PROP_PARAM(Mask, mask, "intersection", 1);

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::Input::MOUSE_HANDLE init_handle;
	Unigine::WidgetLabelPtr current_hit_label{nullptr};
};
