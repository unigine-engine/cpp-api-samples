// Shows the current values of the animated material parameters in the sample description window.
// Every row is refreshed each frame, so the result of each Materials* component is visible
// as a value and not only as a change of the picture on the screen.

#include "MaterialsParametersAnimationSample.h"

#include "MaterialsAlbedoColor.h"
#include "MaterialsAlbedoTexture.h"
#include "MaterialsCastWorldShadow.h"
#include "MaterialsEmission.h"
#include "MaterialsMetalness.h"

#include <UnigineGui.h>
#include <UnigineObjects.h>

REGISTER_COMPONENT(MaterialsParametersAnimationSample);

using namespace Unigine;
using namespace Math;

namespace
{
	// Material of the object a component is attached to, an empty pointer if the component is not in the world
	MaterialPtr get_component_material(ComponentBase *component)
	{
		if (component == nullptr)
			return MaterialPtr();

		ObjectPtr object = checked_ptr_cast<Object>(component->getNode());
		if (object.isValid() == false || object->getNumSurfaces() == 0)
			return MaterialPtr();

		return object->getMaterial(0);
	}
}

// Builds the state group box inside the sample description window.
void MaterialsParametersAnimationSample::init()
{
	description_window.createWindow();

	WidgetGroupBoxPtr state_box = WidgetGroupBox::create("State", 9, 3);
	description_window.getWindow()->addChild(state_box);

	state_grid = WidgetGridBox::create(2);
	state_box->addChild(state_grid, Gui::ALIGN_LEFT);

	// A row is created only for a parameter that is really animated in this world
	albedo_color_material = get_component_material(ComponentSystem::get()->getComponentInWorld<MaterialsAlbedoColor>());
	if (albedo_color_material)
	{
		albedo_color_red_field = add_state_field("albedo_color.r");
		albedo_color_green_field = add_state_field("albedo_color.g");
		albedo_color_blue_field = add_state_field("albedo_color.b");
	}

	albedo_texture_component = ComponentSystem::get()->getComponentInWorld<MaterialsAlbedoTexture>();
	if (albedo_texture_component)
		albedo_texture_field = add_state_field("albedo texture");

	metalness_material = get_component_material(ComponentSystem::get()->getComponentInWorld<MaterialsMetalness>());
	if (metalness_material)
		metalness_field = add_state_field("metalness");

	emission_material = get_component_material(ComponentSystem::get()->getComponentInWorld<MaterialsEmission>());
	if (emission_material)
		emission_field = add_state_field("emission");

	cast_world_shadow_material = get_component_material(ComponentSystem::get()->getComponentInWorld<MaterialsCastWorldShadow>());
	if (cast_world_shadow_material)
		cast_world_shadow_field = add_state_field("cast_world_shadow");
}

// Shows the values the animating components have set this frame.
void MaterialsParametersAnimationSample::update()
{
	if (albedo_color_red_field)
	{
		vec4 color = albedo_color_material->getParameterFloat4("albedo_color");
		albedo_color_red_field->setText(String::format("%.2f", color.x));
		albedo_color_green_field->setText(String::format("%.2f", color.y));
		albedo_color_blue_field->setText(String::format("%.2f", color.z));
	}

	if (albedo_texture_field)
		albedo_texture_field->setText(albedo_texture_component->isFirstTextureApplied() ? "first" : "second");

	if (metalness_field)
		metalness_field->setText(String::format("%.2f", metalness_material->getParameterFloat("metalness")));

	if (emission_field)
		emission_field->setText(emission_material->getState("emission") != 0 ? "on" : "off");

	if (cast_world_shadow_field)
		cast_world_shadow_field->setText(cast_world_shadow_material->isCastWorldShadow() ? "on" : "off");
}

void MaterialsParametersAnimationSample::shutdown()
{
	description_window.shutdown();
}

WidgetEditLinePtr MaterialsParametersAnimationSample::add_state_field(const char *name)
{
	WidgetHBoxPtr name_box = WidgetHBox::create();
	name_box->addChild(WidgetLabel::create(name));
	name_box->addChild(WidgetHBox::create(6));
	state_grid->addChild(name_box, Gui::ALIGN_LEFT);

	WidgetEditLinePtr value_field = WidgetEditLine::create();
	value_field->setEditable(false);
	value_field->setFontVOffset(-2);
	value_field->setFontColor(vec4(vec3(0.9f), 1.0f));
	value_field->setWidth(50);
	state_grid->addChild(value_field, Gui::ALIGN_LEFT);

	return value_field;
}
