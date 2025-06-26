#include "DynamicTextureFieldHeightSample.h"
#include "UnigineGame.h"
#include "UnigineFields.h"
#include "DynamicTextureFieldHeight.h"

REGISTER_COMPONENT(DynamicTextureFieldHeightSample);

using namespace Unigine;
using namespace Math;

void DynamicTextureFieldHeightSample::init()
{
	mouse_grab_state_at_init = Input::isMouseGrab();
	mouse_handle_at_init = Input::getMouseHandle();
	Input::setMouseGrab(false);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	description_window.createWindow();

	WidgetButtonPtr button_size_1024 = WidgetButton::create();
	WidgetButtonPtr button_size_2048 = WidgetButton::create();
	WidgetButtonPtr button_size_4096 = WidgetButton::create();

	button_size_1024->setText("1024");
	button_size_2048->setText("2048");
	button_size_4096->setText("4096");

	button_size_1024->getEventClicked().connect(*this, [this]() {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setImageSize(1024);
			size_map->setText("1024 X 1024");
		}
		});

	button_size_2048->getEventClicked().connect(*this, [this]() {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setImageSize(2048);
			size_map->setText("2048 X 2048");
		}
		});

	button_size_4096->getEventClicked().connect(*this, [this]() {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setImageSize(4096);
			size_map->setText("4096 X 4096");
		}
		});

	WidgetHBoxPtr button_box = WidgetHBox::create();

	button_box->addChild(button_size_1024);
	button_box->addChild(button_size_2048);
	button_box->addChild(button_size_4096);

	description_window.addIntParameter("Size FieldHeight", "", 40, 25, 100, [this](int value) {
		FieldHeightPtr field = checked_ptr_cast<FieldHeight>(field_height.get());
		if (field)
		{
			field->setSize(vec3(value));
		}
		});

	description_window.addFloatParameter("Amplitude", "", 1.0f, 0.0f, 1.0f, [this](float value) {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setAmplitude(value);
		}
		});

	description_window.addFloatParameter("Frequency", "", 1.0f, 1.0f, 40.0f, [this](float value) {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setFrequency(value);
		}
		});

	description_window.addFloatParameter("Speed", "", 1.0f, 0.0f, 20.0f, [this](float value) {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setSpeed(value);
		}
		});

	size_map = WidgetLabel::create();
	size_map->setText("1024 X 1024");
	size_map->setFontSize(20);
	DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);

	WidgetSpritePtr sprite = WidgetSprite::create();
	sprite->setHeight(200);
	sprite->setWidth(200);
	component->setWidgetSprite(sprite);


	description_window.addParameterSpacer();

	description_window.getParameterGroupBox()->addChild(button_box, Gui::ALIGN_LEFT);

	WidgetGroupBoxPtr image_group = WidgetGroupBox::create();
	image_group->addChild(size_map);
	image_group->addChild(sprite);

	description_window.getParameterGroupBox()->addChild(button_box, Gui::ALIGN_TOP);
	description_window.getParameterGroupBox()->addChild(image_group, Gui::ALIGN_TOP);
}

void DynamicTextureFieldHeightSample::shutdown()
{
	Input::setMouseGrab(mouse_grab_state_at_init);
	Input::setMouseHandle(mouse_handle_at_init);

	description_window.shutdown();
}
