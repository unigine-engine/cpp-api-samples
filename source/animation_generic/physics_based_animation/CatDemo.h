#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class MotionMode;
class SpringRegular;

// Demo mode controller for the cat chasing sample.
// Manages switching between different motion interpolation modes
// (Linear, EaseIn, EaseOut, Spring, etc.) via a dropdown menu.
// Each child node represents a different easing algorithm.
class CatDemo : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(CatDemo, Unigine::ComponentBase);

	COMPONENT_INIT(init, -1);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Node, catNode);
	PROP_PARAM(Node, laserNode);

	// Add demo-related GUI elements to the sample window
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
