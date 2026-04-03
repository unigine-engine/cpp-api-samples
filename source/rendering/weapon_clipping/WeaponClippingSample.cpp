// Sample UI for weapon clipping settings. Provides toggle to skip shadow
// rendering in the weapon layer for performance optimization.

#include "WeaponClippingSample.h"
#include "WeaponClipping.h"

using namespace Unigine;

REGISTER_COMPONENT(WeaponClippingSample);

// UI checkbox is created to toggle shadow rendering in the weapon layer.
void WeaponClippingSample::init()
{
	// Get reference to WeaponClipping component on the linked node
	WeaponClipping* weapon_clipping_component = ComponentSystem::get()->getComponent<WeaponClipping>(weapon_clipping_node);

	if (!weapon_clipping_component)
		return;

	sample_description_window.createWindow();

	// Build checkbox UI for shadow skip toggle
	auto parameters = sample_description_window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	auto checkbox = WidgetCheckBox::create();

	// Toggle SKIP_SHADOWS flag on weapon viewport when checkbox state changes
	checkbox->getEventClicked().connect(*this, [weapon_clipping_component, this, checkbox]() {
		// Always skip velocity buffer; optionally skip shadows for performance
		int flags = Viewport::SKIP_VELOCITY_BUFFER;
		if (checkbox->isChecked())
			flags |= Viewport::SKIP_SHADOWS;

		weapon_clipping_component->getRenderViewport()->setSkipFlags(flags);
	});

	auto label = WidgetLabel::create("Skip shadows");

	hbox->addChild(label);
	hbox->addChild(checkbox);

	parameters->addChild(hbox, Gui::ALIGN_LEFT);
}

// Sample UI window is cleaned up on component destruction.
void WeaponClippingSample::shutdown()
{
	sample_description_window.shutdown();
}

