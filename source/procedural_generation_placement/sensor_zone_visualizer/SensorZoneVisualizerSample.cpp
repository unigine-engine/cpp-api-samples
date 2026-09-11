#include "SensorZoneVisualizerSample.h"

#include <UnigineLog.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SensorZoneVisualizerSample);

void SensorZoneVisualizerSample::init()
{
	// Each direction's visualizer lives on its own node, referenced explicitly.
	// Order is clockwise from forward (top-down view): forward, right, back, left.
	sensors[0].visualizer = getComponent<SensorZoneVisualizer>(sensor_forward.get());
	sensors[1].visualizer = getComponent<SensorZoneVisualizer>(sensor_right.get());
	sensors[2].visualizer = getComponent<SensorZoneVisualizer>(sensor_back.get());
	sensors[3].visualizer = getComponent<SensorZoneVisualizer>(sensor_left.get());

	for (int i = 0; i < NUM_SENSORS; i++)
	{
		SensorZoneVisualizer *visualizer = sensors[i].visualizer;
		if (!visualizer)
		{
			Log::warning("SensorZoneVisualizerSample::init: sensor zone node %d is not assigned\n", i);
			continue;
		}

		// start from the current values: angles from the property, color from the
		// visualizer's material (its current "Emission Color")
		sensors[i].horizontal_angles = visualizer->horizontal_angles.get();
		sensors[i].vertical_angles = visualizer->vertical_angles.get();
		vec4 material_color = visualizer->getColor();
		sensors[i].color = vec3(material_color.x, material_color.y, material_color.z);
	}

	build_ui();
}

void SensorZoneVisualizerSample::update()
{
	// The color dialog has no "changed" event, so while it is open we poll its
	// color and apply it immediately whenever it differs from the last applied one.
	if (!color_dialog || !active_color_change)
		return;

	vec4 color = color_dialog->getColor();
	if (color != active_color_last)
	{
		active_color_last = color;
		active_color_change(color);
		if (active_swatch)
			active_swatch->setColor(color);
	}
}

void SensorZoneVisualizerSample::shutdown()
{
	close_color_dialog();
	if (tab_box)
		tab_box.deleteLater();
	description_window.shutdown();
}

void SensorZoneVisualizerSample::close_color_dialog()
{
	color_dialog_connections.disconnectAll();
	active_color_change = nullptr;
	active_swatch.clear();
	if (color_dialog)
		color_dialog.deleteLater();
}

void SensorZoneVisualizerSample::build_ui()
{
	description_window.createWindow();

	// One tab per sensor direction, placed inside the "Parameters" group.
	tab_box = WidgetTabBox::create(4, 4);
	description_window.getParameterGroupBox()->addChild(tab_box, Gui::ALIGN_EXPAND);

	for (int i = 0; i < NUM_SENSORS; i++)
		build_tab(i);

	// start on the forward sensor
	tab_box->setCurrentTab(0);
}

void SensorZoneVisualizerSample::build_tab(int index)
{
	static const char *names[NUM_SENSORS] = {"Forward", "Right", "Back", "Left"};
	tab_box->addTab(names[index]);

	SensorZoneVisualizer *visualizer = sensors[index].visualizer;
	if (!visualizer)
	{
		// keep the tab present but inform that its node is missing
		tab_box->addChild(WidgetLabel::create("No sensor zone node assigned"), Gui::ALIGN_LEFT);
		return;
	}

	WidgetGridBoxPtr grid = WidgetGridBox::create(3);
	grid->setSpace(4, 4);
	tab_box->addChild(grid, Gui::ALIGN_EXPAND);

	// Near / far distance (radii of the spherical caps). Near can't move above
	// far; far can't move below near.
	sensors[index].near_slider = add_float_slider(grid, "Near distance", "Near radius of the zone",
		visualizer->near.get(), 0.1f, 10.0f, [this, index](float value) {
			if (value > sensors[index].visualizer->far.get() - MIN_GAP)
			{
				value = sensors[index].visualizer->far.get() - MIN_GAP;
				MUTE_EVENT(sensors[index].near_slider->getEventChanged());
				sensors[index].near_slider->setValue((int)(value * 100));
			}
			sensors[index].visualizer->near = value;
			sensors[index].visualizer->refresh();
		});

	sensors[index].far_slider = add_float_slider(grid, "Far distance", "Far radius of the zone",
		visualizer->far.get(), 0.2f, 20.0f, [this, index](float value) {
			if (value < sensors[index].visualizer->near.get() + MIN_GAP)
			{
				value = sensors[index].visualizer->near.get() + MIN_GAP;
				MUTE_EVENT(sensors[index].far_slider->getEventChanged());
				sensors[index].far_slider->setValue((int)(value * 100));
			}
			sensors[index].visualizer->far = value;
			sensors[index].visualizer->refresh();
		});

	// Horizontal angles (yaw). Each border stays in its own half relative to the
	// forward direction: left in [-180, 0], right in [0, 180], so they never cross.
	sensors[index].horizontal_min = add_float_slider(grid, "Left border", "Minimum horizontal angle, degrees",
		sensors[index].horizontal_angles.x, -180.0f, 0.0f, [this, index](float value) {
			if (value > sensors[index].horizontal_angles.y - MIN_GAP)
			{
				value = sensors[index].horizontal_angles.y - MIN_GAP;
				MUTE_EVENT(sensors[index].horizontal_min->getEventChanged());
				sensors[index].horizontal_min->setValue((int)(value * 100));
			}
			sensors[index].horizontal_angles.x = value;
			sensors[index].visualizer->horizontal_angles = sensors[index].horizontal_angles;
			sensors[index].visualizer->refresh();
		});

	sensors[index].horizontal_max = add_float_slider(grid, "Right border", "Maximum horizontal angle, degrees",
		sensors[index].horizontal_angles.y, 0.0f, 180.0f, [this, index](float value) {
			if (value < sensors[index].horizontal_angles.x + MIN_GAP)
			{
				value = sensors[index].horizontal_angles.x + MIN_GAP;
				MUTE_EVENT(sensors[index].horizontal_max->getEventChanged());
				sensors[index].horizontal_max->setValue((int)(value * 100));
			}
			sensors[index].horizontal_angles.y = value;
			sensors[index].visualizer->horizontal_angles = sensors[index].horizontal_angles;
			sensors[index].visualizer->refresh();
		});

	// Vertical angles (pitch). Same half-space split: lower in [-180, 0], upper in
	// [0, 180], built from the forward direction without crossing into each other.
	sensors[index].vertical_min = add_float_slider(grid, "Lower border", "Minimum vertical angle, degrees",
		sensors[index].vertical_angles.x, -180.0f, 0.0f, [this, index](float value) {
			if (value > sensors[index].vertical_angles.y - MIN_GAP)
			{
				value = sensors[index].vertical_angles.y - MIN_GAP;
				MUTE_EVENT(sensors[index].vertical_min->getEventChanged());
				sensors[index].vertical_min->setValue((int)(value * 100));
			}
			sensors[index].vertical_angles.x = value;
			sensors[index].visualizer->vertical_angles = sensors[index].vertical_angles;
			sensors[index].visualizer->refresh();
		});

	sensors[index].vertical_max = add_float_slider(grid, "Upper border", "Maximum vertical angle, degrees",
		sensors[index].vertical_angles.y, 0.0f, 180.0f, [this, index](float value) {
			if (value < sensors[index].vertical_angles.x + MIN_GAP)
			{
				value = sensors[index].vertical_angles.x + MIN_GAP;
				MUTE_EVENT(sensors[index].vertical_max->getEventChanged());
				sensors[index].vertical_max->setValue((int)(value * 100));
			}
			sensors[index].vertical_angles.y = value;
			sensors[index].visualizer->vertical_angles = sensors[index].vertical_angles;
			sensors[index].visualizer->refresh();
		});

	// Color drives the material "Emission Color" (visible only when the zone
	// material exposes that parameter). The swatch opens a color dialog.
	add_color_picker(grid, "Color", vec4(sensors[index].color, 1.0f),
		[this, index](const vec4 &color) {
			sensors[index].color = vec3(color.x, color.y, color.z);
			sensors[index].visualizer->setColor(vec4(sensors[index].color, 1.0f));
		});
}

// Builds a labelled float slider (label + slider + value readout) in the grid.
// Mirrors SampleDescriptionWindow::addFloatParameter: the integer slider keeps
// two decimals of precision via a x100 scale.
WidgetSliderPtr SensorZoneVisualizerSample::add_float_slider(const WidgetGridBoxPtr &grid, const char *name,
	const char *tooltip, float value, float min_value, float max_value, std::function<void(float)> on_change)
{
	WidgetLabelPtr label = WidgetLabel::create(name);
	label->setWidth(100);
	label->setToolTip(tooltip);
	grid->addChild(label, Gui::ALIGN_LEFT);

	WidgetSliderPtr slider = WidgetSlider::create();
	slider->setMinValue((int)(min_value * 100));
	slider->setMaxValue((int)(max_value * 100));
	slider->setValue((int)(value * 100));
	slider->setWidth(200);
	slider->setButtonWidth(20);
	slider->setButtonHeight(20);
	slider->setToolTip(tooltip);
	grid->addChild(slider, Gui::ALIGN_LEFT);

	WidgetLabelPtr value_label = WidgetLabel::create(String::ftoa(value, 2));
	value_label->setWidth(40);
	value_label->setToolTip(tooltip);
	grid->addChild(value_label);

	// on_change may clamp by re-setting the slider, so read the final value back
	// for the readout label.
	slider->getEventChanged().connect(*this, [slider, value_label, on_change]() {
		on_change(slider->getValue() / 100.0f);
		value_label->setText(String::ftoa(slider->getValue() / 100.0f, 2));
	});

	return slider;
}

// A color swatch (label + clickable sprite). Clicking it opens a WidgetDialogColor;
// on OK the picked color is applied to the swatch and passed to on_change.
void SensorZoneVisualizerSample::add_color_picker(const WidgetGridBoxPtr &grid, const char *name,
	vec4 init_color, std::function<void(const vec4 &)> on_change)
{
	WidgetLabelPtr label = WidgetLabel::create(name);
	label->setWidth(100);
	grid->addChild(label, Gui::ALIGN_LEFT);

	WidgetSpritePtr swatch = WidgetSprite::create();
	// match the slider width so the color row lines up with the parameter rows
	swatch->setWidth(200);
	swatch->setTexture("core/textures/common/white.texture");
	swatch->setColor(init_color);
	grid->addChild(swatch, Gui::ALIGN_LEFT);
	// the third grid column is unused for this row
	grid->addChild(WidgetLabel::create(""));

	swatch->getEventClicked().connect(*this, [this, swatch, on_change]() {
		// only one dialog at a time: drop the previous one and its connections
		close_color_dialog();

		color_dialog = WidgetDialogColor::create("Select color");
		color_dialog->setColor(swatch->getColor());

		// remember what this dialog edits so update() can apply changes live and
		// Cancel can revert to the color the swatch had before opening
		active_swatch = swatch;
		active_color_change = on_change;
		active_color_original = swatch->getColor();
		active_color_last = swatch->getColor();

		// the color is already applied live; OK just closes the dialog
		color_dialog->getOkButton()->getEventClicked().connect(color_dialog_connections,
			[this]() { close_color_dialog(); });
		// Cancel restores the original color, then closes
		color_dialog->getCancelButton()->getEventClicked().connect(color_dialog_connections,
			[this]() {
				if (active_color_change)
					active_color_change(active_color_original);
				if (active_swatch)
					active_swatch->setColor(active_color_original);
				close_color_dialog();
			});

		swatch->getGui()->addChild(color_dialog, Gui::ALIGN_OVERLAP);
		color_dialog->setPermanentFocus();
	});
}
