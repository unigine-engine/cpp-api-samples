#pragma once

#include "UnigineComponentSystem.h"

#include <UnigineNode.h>
#include <UnigineVisualizer.h>


class ObjectFrame : public Unigine::ComponentBase
{
	COMPONENT_DEFINE(ObjectFrame, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, 2);
	COMPONENT_POST_UPDATE(post_update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Int, border_size, 3);
	PROP_PARAM(Color, border_color, Unigine::Math::vec4_blue);

	PROP_PARAM(String, label_text, "Name");
	PROP_PARAM(Int, label_text_size, 15);
	PROP_PARAM(Int, label_text_outline, 1);
	PROP_PARAM(Color, label_text_color, Unigine::Math::vec4_white);

	PROP_PARAM(Float, visibility_distance, 40.f);

public:
	static void setObjectFramesEnabled(bool enabled);
	bool isVisible() const { return is_frame_rendered; }
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
	static Unigine::WidgetCanvasPtr canvas;
	static bool canvas_clean;
	static int components_count;

	Unigine::Math::BoundBox bound_box;
	Unigine::VectorStack<Unigine::Math::vec3, 8> points;
	Unigine::Math::vec3 local_center;

	float max_visibility_distance;

	bool is_frame_rendered;
	Unigine::Math::ivec2 min_point;
	Unigine::Math::ivec2 max_point;
};
