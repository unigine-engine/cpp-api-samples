// Provides UI controls for the DynamicTextureFieldHeight sample. Allows runtime
// adjustment of heightmap resolution, wave amplitude, frequency, and animation speed.
// Displays a preview sprite showing the generated heightmap texture.

#include "DynamicTextureFieldHeightSample.h"
#include "UnigineGame.h"
#include "UnigineFields.h"
#include "DynamicTextureFieldHeight.h"

REGISTER_COMPONENT(DynamicTextureFieldHeightSample);

using namespace Unigine;
using namespace Math;

void DynamicTextureFieldHeightSample::init()
{
	description_window.createWindow();

	// Resolution selection buttons for heightmap texture
	// Higher resolution = more detail but higher memory and CPU cost
	WidgetButtonPtr button_size_1024 = WidgetButton::create();
	WidgetButtonPtr button_size_2048 = WidgetButton::create();
	WidgetButtonPtr button_size_4096 = WidgetButton::create();

	button_size_1024->setText("1024");
	button_size_2048->setText("2048");
	button_size_4096->setText("4096");

	// getComponent<T> retrieves a component from a node by type
	// Returns raw pointer (not smart pointer) for component access
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

	// Horizontal layout for resolution buttons
	WidgetHBoxPtr button_box = WidgetHBox::create();

	button_box->addChild(button_size_1024);
	button_box->addChild(button_size_2048);
	button_box->addChild(button_size_4096);

	// Field size controls the world-space extent of height modification
	description_window.addIntParameter("Size FieldHeight", "", 40, 25, 100, [this](int value) {
		FieldHeightPtr field = checked_ptr_cast<FieldHeight>(field_height.get());
		if (field)
		{
			// Uniform scale in all axes
			field->setSize(vec3(float(value)));
		}
		});

	// Amplitude: wave height intensity (0 = flat, 1 = maximum)
	description_window.addFloatParameter("Amplitude", "", 1.0f, 0.0f, 1.0f, [this](float value) {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setAmplitude(value);
		}
		});

	// Frequency: number of wave cycles across texture
	description_window.addFloatParameter("Frequency", "", 1.0f, 1.0f, 40.0f, [this](float value) {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setFrequency(value);
		}
		});

	// Speed: animation rate multiplier
	description_window.addFloatParameter("Speed", "", 1.0f, 0.0f, 20.0f, [this](float value) {
		DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);
		if (component)
		{
			component->setSpeed(value);
		}
		});

	// Label displaying current texture dimensions
	size_map = WidgetLabel::create();
	size_map->setText("1024 X 1024");
	size_map->setFontSize(20);
	DynamicTextureFieldHeight* component = getComponent<DynamicTextureFieldHeight>(field_height);

	// Create sprite widget to preview the heightmap texture
	// The sprite receives image updates from DynamicTextureFieldHeight::update()
	WidgetSpritePtr sprite = WidgetSprite::create();
	sprite->setHeight(200);
	sprite->setWidth(200);
	component->setWidgetSprite(sprite);


	description_window.addParameterSpacer();

	description_window.getParameterGroupBox()->addChild(button_box, Gui::ALIGN_LEFT);

	// Group box containing resolution label and preview sprite
	WidgetGroupBoxPtr image_group = WidgetGroupBox::create();
	image_group->addChild(size_map);
	image_group->addChild(sprite);

	description_window.getParameterGroupBox()->addChild(button_box, Gui::ALIGN_TOP);
	description_window.getParameterGroupBox()->addChild(image_group, Gui::ALIGN_TOP);
}

void DynamicTextureFieldHeightSample::shutdown()
{
	description_window.shutdown();
}
