// Renders 2D selection frames around 3D objects using WidgetCanvas.
// Bounding box corners are projected to screen space with visibility culling.
// Supports configurable border style, label text, and distance-based visibility.

#pragma once

#include "UnigineComponentSystem.h"

#include <UnigineNode.h>
#include <UnigineVisualizer.h>

// Projects 3D object bounds to screen and draws rectangular selection frame.
class ObjectFrame : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ObjectFrame, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, 2);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_SHUTDOWN(shutdown);

	// Frame border thickness in pixels
	PROP_PARAM(Int, border_size, 3);
	// Color of the frame border
	PROP_PARAM(Color, border_color, Unigine::Math::vec4_blue);

	// Text displayed above the frame
	PROP_PARAM(String, label_text, "Name");
	// Font size for the label
	PROP_PARAM(Int, label_text_size, 15);
	// Outline thickness for label text
	PROP_PARAM(Int, label_text_outline, 1);
	// Color of the label text
	PROP_PARAM(Color, label_text_color, Unigine::Math::vec4_white);

	// Maximum distance at which frame is rendered
	PROP_PARAM(Float, visibility_distance, 40.f);

public:
	// Globally enables/disables all object frames
	static void setObjectFramesEnabled(bool enabled);
	// Returns whether frame was rendered this frame
	bool isVisible() const { return is_frame_rendered; }
	// Returns JSON with node position, rotation, and screen bounds
	Unigine::JsonPtr getJsonMeta() const;

private:
	void init();
	void update();
	void post_update();
	void shutdown();

	void collect_mesh_bound_box(const Unigine::NodePtr &n);
	bool get_screen_position(int &x, int &y, const Unigine::Math::Vec3 &world_point,
		const Unigine::PlayerPtr &player, const Unigine::Math::ivec2 &screen_size);
	int draw_rectangle(int order, const Unigine::Math::ivec2 &p1, const Unigine::Math::ivec2 &p2);
	bool is_rectangle_on_screen(const Unigine::Math::ivec2 &screen_size,
		const Unigine::Math::ivec2 &min, const Unigine::Math::ivec2 &max);
	bool is_child(const Unigine::NodePtr &n);

private:
	// Shared canvas for all ObjectFrame instances
	static Unigine::WidgetCanvasPtr canvas;
	// Flag to track if canvas was cleared this frame
	static bool canvas_clean;
	// Reference count for managing shared canvas lifetime
	static int components_count;

	// Combined bounding box of all mesh children
	Unigine::Math::BoundBox bound_box;
	// Eight corners of the bounding box
	Unigine::VectorStack<Unigine::Math::vec3, 8> points;
	// Center point of bounding box in local space
	Unigine::Math::vec3 local_center;

	// Squared visibility distance for fast comparison
	float max_visibility_distance;

	// Whether frame was successfully rendered this frame
	bool is_frame_rendered;
	// Screen-space bounds of the rendered frame
	Unigine::Math::ivec2 min_point;
	Unigine::Math::ivec2 max_point;
};
