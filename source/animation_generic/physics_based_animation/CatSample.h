#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class CatDemo;
class CatGame;

// Main sample component demonstrating physics-based animation with easing functions.
// Shows a cat chasing a laser pointer using different motion interpolation methods:
// Linear, EaseIn, EaseOut, EaseOutBounce, and Spring-based motion.
// Two modes available: Demo (automatic showcase) and Game (interactive laser control).
class CatSample: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CatSample, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, demoNode);	// Root node for demo mode
	PROP_PARAM(Node, gameNode);	// Root node for game mode

private:
	CatDemo *demo_manager;
	CatGame *game_manager;

	SampleDescriptionWindow sample_description_window;
	Unigine::WidgetLabelPtr title_label;
	Unigine::WidgetButtonPtr demo_button;
	Unigine::WidgetButtonPtr game_button;

	Unigine::Input::MOUSE_HANDLE mouse_handle;

private:
	void init();
	void init_gui();
	void switch_to_game();
	void switch_to_demo();
	void shutdown();
	void shutdown_gui();
};



