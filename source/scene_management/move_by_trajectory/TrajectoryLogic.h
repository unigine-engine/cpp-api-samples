// Sample UI for trajectory movement demonstration with three airplanes.
// Each airplane uses a different movement approach (linear, spline, path file).
// Provides velocity control, camera switching, and path visualization toggle.

#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "UnigineComponentSystem.h"
#include "UniginePlayers.h"

// Controls sample UI and manages camera switching between airplanes.
class TrajectoryLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrajectoryLogic, ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the sample and demonstrates "
						  "how to use a trajectory movement.");
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Airplane using linear interpolation
	PROP_PARAM(Node, airplane_1);
	// Airplane using spline interpolation
	PROP_PARAM(Node, airplane_2);
	// Airplane using saved path file
	PROP_PARAM(Node, airplane_3);

private:
	void init();
	void update();
	void shutdown();
	void init_gui();
	// Updates velocity across all movement components
	void update_velocity_callback();
	// Cycles active camera through main and airplane-attached views
	void switch_trajectory_callback();
	// Toggles debug path visualization on all airplanes
	void enable_visualize_callback();

	Unigine::WidgetWindowPtr window;

	Unigine::WidgetSliderPtr velocity_widget;
	Unigine::WidgetLabelPtr velocity_value_label;
	Unigine::WidgetButtonPtr switch_camera;
	Unigine::WidgetCheckBoxPtr enable_visualize_path;

	// Camera indices for switching between views
	enum Players
	{
		MAIN,
		ONE,
		TWO,
		THREE,
		TOTAL_PLAYERS
	};

	// All available cameras (main plus airplane-attached)
	Unigine::Vector<Unigine::PlayerPtr> main_players;
	Players current_player = MAIN;

	SampleDescriptionWindow sample_description_window;
};
