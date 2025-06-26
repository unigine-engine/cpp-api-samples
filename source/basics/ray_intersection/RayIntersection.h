#pragma once

#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class RayIntersection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(RayIntersection, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	PROP_PARAM(Node, laser_ray, nullptr);
	PROP_PARAM(Node, laser_hit, nullptr);
	PROP_PARAM(Float, laser_distance, 25.0f);
	PROP_PARAM(Mask, intersection_mask, "intersection", 1);

private:
	Unigine::WorldIntersectionNormalPtr intersection;
	Unigine::Math::vec3 laser_ray_scale = Unigine::Math::vec3_one;

	// used for GUI
	Unigine::String prev_text = "";
	Unigine::ObjectGuiPtr mask_text = nullptr;
	SampleDescriptionWindow window;
	Unigine::WidgetLabelPtr current_hit;
	Unigine::Vector<const char *> walls{
		"RED", "GREEN", "BLUE", "BLACK", "WHITE", "YELLOW", "GRAY", "ORANGE"};
	Unigine::Vector<const char *> colors{
		"#FF4D4D", // RED — bright red
		"#00ff7f", // GREEN — bright green (spring green)
		"#3399ff", // BLUE — light blue
		"#000000", // BLACK — pure black
		"#ffffff", // WHITE — pure white
		"#ffff66", // YELLOW — light yellow (lemon)
		"#cccccc", // GRAY — light gray
		"#ff9933"  // ORANGE — bright orange
	};

	void init_gui();
	
	void init();
	void update();
	void shutdown();

	Unigine::String current_node;
};