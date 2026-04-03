// Demonstrates ray-based intersection testing with mask filtering.
// A laser ray is cast each frame, and colored walls can be selectively
// included or excluded via intersection mask checkboxes.

#pragma once

#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

// Casts a laser ray and visualizes hits with mask-based filtering UI.
class RayIntersection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(RayIntersection, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)

	// Laser beam visual that stretches to hit point
	PROP_PARAM(Node, laser_ray, nullptr);
	// Hit point indicator positioned at intersection
	PROP_PARAM(Node, laser_hit, nullptr);
	// Maximum ray length when no intersection occurs
	PROP_PARAM(Float, laser_distance, 25.0f);
	// Bitmask determining which objects can be hit
	PROP_PARAM(Mask, intersection_mask, "intersection", 1);

private:
	// Stores intersection point and normal for hit visualization
	Unigine::WorldIntersectionNormalPtr intersection;
	// Original scale cached for dynamic length adjustment
	Unigine::Math::vec3 laser_ray_scale = Unigine::Math::vec3_one;

	// GUI state for binary mask input validation
	Unigine::String prev_text = "";
	Unigine::ObjectGuiPtr mask_text = nullptr;
	SampleDescriptionWindow window;
	Unigine::WidgetLabelPtr current_hit;
	// Wall names for each bit position in the mask
	Unigine::Vector<const char *> walls{
		"RED", "GREEN", "BLUE", "BLACK", "WHITE", "YELLOW", "GRAY", "ORANGE"};
	// HTML colors for rich text display
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