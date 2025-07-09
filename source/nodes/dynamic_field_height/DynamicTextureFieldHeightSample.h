#pragma once
#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

class DynamicTextureFieldHeightSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(DynamicTextureFieldHeightSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, field_height, "", "", "", "filter=FieldHeight");

private:
	void init();
	void shutdown();

private:

	SampleDescriptionWindow description_window;

	Unigine::WidgetLabelPtr size_map;
};
