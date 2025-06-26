#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class MotionMode;
class SpringRegular;

class CatDemo : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CatDemo, Unigine::ComponentBase);

	COMPONENT_INIT(init, -1);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, catNode);
	PROP_PARAM(Node, laserNode);

	// add to window game related GUI
	void initGUI(const Unigine::WidgetWindowPtr &window);

private:
	int mode = 0;

	Unigine::WidgetGroupBoxPtr demo_box;
	Unigine::WidgetComboBoxPtr demo_combo;
	Unigine::Vector<MotionMode *> cat_components;

private:
	void init();
	void on_enable() override;
	void on_disable() override;
	void update_cat_mode();
	void switch_cat_mode(int mode);
	void shutdown();
	void shutdown_gui();
};
