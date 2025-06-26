#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class Laser;
class SpringRegular;

class CatGame : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CatGame, Unigine::ComponentBase);

	COMPONENT_INIT(init, -1);
	// update CatGame after updating laser and cat's movement components
	COMPONENT_UPDATE(update, 1);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, laserNode);
	PROP_PARAM(Node, catMode);

	// add to window game related GUI
	void initGUI(const Unigine::WidgetWindowPtr &window);

private:
	float timer = 0.f;
	bool catched = false;

	SpringRegular *cat_mode = nullptr;
	Laser *laser_component = nullptr;

	Unigine::WidgetGroupBoxPtr game_box;
	Unigine::WidgetLabelPtr game_label;
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();

private:
	void init();
	void on_enable() override;
	void on_disable() override;
	void update();
	void update_game();
	void update_gameover();
	void shutdown();
	void shutdown_gui();
};
