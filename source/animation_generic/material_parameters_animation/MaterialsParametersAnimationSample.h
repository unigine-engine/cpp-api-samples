#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineMaterial.h>
#include <UnigineWidgets.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class MaterialsAlbedoTexture;

// Displays the state of the material parameters animated by the Materials* components.
class MaterialsParametersAnimationSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MaterialsParametersAnimationSample, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

	// A read-only row "parameter name | current value" in the state grid
	Unigine::WidgetEditLinePtr add_state_field(const char *name);

private:
	SampleDescriptionWindow description_window;
	Unigine::WidgetGridBoxPtr state_grid;

	// Materials of the objects the animating components are attached to
	Unigine::MaterialPtr albedo_color_material;
	Unigine::MaterialPtr metalness_material;
	Unigine::MaterialPtr emission_material;
	Unigine::MaterialPtr cast_world_shadow_material;

	// The texture component knows which of its two textures is applied right now
	MaterialsAlbedoTexture *albedo_texture_component = nullptr;

	Unigine::WidgetEditLinePtr albedo_color_red_field;
	Unigine::WidgetEditLinePtr albedo_color_green_field;
	Unigine::WidgetEditLinePtr albedo_color_blue_field;
	Unigine::WidgetEditLinePtr albedo_texture_field;
	Unigine::WidgetEditLinePtr metalness_field;
	Unigine::WidgetEditLinePtr emission_field;
	Unigine::WidgetEditLinePtr cast_world_shadow_field;
};
