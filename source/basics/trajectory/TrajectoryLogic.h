#pragma once
#include "../../menu_ui/SampleDescriptionWindow.h"
#include "UnigineComponentSystem.h"
#include "UniginePlayers.h"

class TrajectoryLogic : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrajectoryLogic, ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the sample and demonstrates "
						  "how to use a trajectory movement.");
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, airplane_1);
	PROP_PARAM(Node, airplane_2);
	PROP_PARAM(Node, airplane_3);

private:
	void init();
	void update();
	void shutdown();

	void init_gui();
	void update_velocity_callback();
	void switch_trajectory_callback();
	void enable_visualize_callback();

	Unigine::WidgetWindowPtr window;

	Unigine::WidgetSliderPtr velocity_widget;
	Unigine::WidgetLabelPtr velocity_value_label;
	Unigine::WidgetButtonPtr switch_camera;
	Unigine::WidgetCheckBoxPtr enable_visualize_path;

	enum Players
	{
		MAIN,
		ONE,
		TWO,
		THREE,
		TOTAL_PLAYERS
	};

	Unigine::Vector<Unigine::PlayerPtr> main_players;
	Players current_player = MAIN;

	SampleDescriptionWindow sample_description_window;
};
