// Provides UI controls for the DynamicTextureFieldHeight sample. Allows runtime
// adjustment of heightmap resolution, wave amplitude, frequency, and animation speed.
// Displays a preview sprite showing the generated heightmap texture.

#pragma once
#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

using namespace Unigine;
using namespace Math;

class DynamicTextureFieldHeightSample final : public Unigine::ComponentBase {
	COMPONENT_DEFINE(DynamicTextureFieldHeightSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	// Reference to the FieldHeight node with DynamicTextureFieldHeight component
	// The filter ensures only FieldHeight nodes can be assigned in the editor
	PROP_PARAM(Node, field_height, "", "", "", "filter=FieldHeight");

private:
	void init();
	void shutdown();

private:

	SampleDescriptionWindow description_window;

	// Label showing current texture resolution (e.g., "1024 X 1024")
	Unigine::WidgetLabelPtr size_map;
};
