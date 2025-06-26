#include "WeaponClippingSample.h"
#include "WeaponClipping.h"

using namespace Unigine;

REGISTER_COMPONENT(WeaponClippingSample);

void WeaponClippingSample::init()
{
	WeaponClipping* weapon_clipping_component = ComponentSystem::get()->getComponent<WeaponClipping>(weapon_clipping_node);

	if (!weapon_clipping_component)
		return;

	sample_description_window.createWindow();

	auto parameters = sample_description_window.getParameterGroupBox();
	auto hbox = WidgetHBox::create();
	auto checkbox = WidgetCheckBox::create();
	checkbox->getEventClicked().connect(*this, [weapon_clipping_component, this, checkbox]() {
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

void WeaponClippingSample::shutdown()
{
	sample_description_window.shutdown();
}

