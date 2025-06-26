#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class CatDemo;
class CatGame;

class CatSample: public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CatSample, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, demoNode);
	PROP_PARAM(Node, gameNode);

private:
	CatDemo *demo_manager;
	CatGame *game_manager;

	SampleDescriptionWindow sample_description_window;
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



