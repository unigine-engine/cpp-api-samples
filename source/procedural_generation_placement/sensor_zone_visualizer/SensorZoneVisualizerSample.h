#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

#include <functional>

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "SensorZoneVisualizer.h"

// Drives four SensorZoneVisualizers mounted on one object (forward / back / left /
// right) to visualize the field of view of four sensors. The parameter panel
// shows one tab per direction; sliders for near/far distance, horizontal and
// vertical angles and color rebuild that zone's mesh online.
class SensorZoneVisualizerSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SensorZoneVisualizerSample, Unigine::ComponentBase);
	// init order 2 so it runs after the SensorZoneVisualizers (order 1) and their
	// material color is already available via getColor()
	COMPONENT_INIT(init, 2);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	static const int NUM_SENSORS = 4;
	// minimal gap kept between a min/max slider pair so they never meet
	static constexpr float MIN_GAP = 0.01f;

	// Clockwise from forward (top-down view): forward, right, back, left.
	PROP_PARAM(Node, sensor_forward, "Forward sensor", "Node with the forward SensorZoneVisualizer");
	PROP_PARAM(Node, sensor_right, "Right sensor", "Node with the right SensorZoneVisualizer");
	PROP_PARAM(Node, sensor_back, "Back sensor", "Node with the back SensorZoneVisualizer");
	PROP_PARAM(Node, sensor_left, "Left sensor", "Node with the left SensorZoneVisualizer");

	void init();
	void update();
	void shutdown();

	void build_ui();
	void build_tab(int index);
	Unigine::WidgetSliderPtr add_float_slider(const Unigine::WidgetGridBoxPtr &grid, const char *name,
		const char *tooltip, float value, float min_value, float max_value, std::function<void(float)> on_change);
	// A clickable color swatch that opens a color dialog; on_change fires on OK.
	void add_color_picker(const Unigine::WidgetGridBoxPtr &grid, const char *name,
		Unigine::Math::vec4 init_color, std::function<void(const Unigine::Math::vec4 &)> on_change);

	// One editable sensor zone per direction. Angles/color are mirrored here so the
	// paired min/max sliders can rebuild the vec2 ranges on change.
	struct SensorUI
	{
		SensorZoneVisualizer *visualizer = nullptr;
		Unigine::Math::vec2 horizontal_angles;
		Unigine::Math::vec2 vertical_angles;
		Unigine::Math::vec3 color;
		// kept so a min/max slider can snap itself back when it would cross its pair
		Unigine::WidgetSliderPtr near_slider;
		Unigine::WidgetSliderPtr far_slider;
		Unigine::WidgetSliderPtr horizontal_min;
		Unigine::WidgetSliderPtr horizontal_max;
		Unigine::WidgetSliderPtr vertical_min;
		Unigine::WidgetSliderPtr vertical_max;
	};
	SensorUI sensors[NUM_SENSORS];

	SampleDescriptionWindow description_window;
	Unigine::WidgetTabBoxPtr tab_box;

	// The shared color dialog: only one is open at a time, reopened per swatch.
	// While it is open the picked color is applied live (polled in update()).
	Unigine::WidgetDialogColorPtr color_dialog;
	Unigine::EventConnections color_dialog_connections;
	Unigine::WidgetSpritePtr active_swatch;
	std::function<void(const Unigine::Math::vec4 &)> active_color_change;
	Unigine::Math::vec4 active_color_original;
	Unigine::Math::vec4 active_color_last;

	void close_color_dialog();
};
