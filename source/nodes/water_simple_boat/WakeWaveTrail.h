#pragma once

#include <UnigineDecals.h>
#include <UniginePair.h>
#include <UnigineComponentSystem.h>

using namespace Unigine;
using namespace Math;


class WakeWaveTrail: public ComponentBase
{
	struct PointInfo
	{
		Vec3 point;
		vec3 right;
		vec3 forward;
		float time = 0.0f;
		float distance = 0.0f;
		float speed = 0.0f;
	};
public:
	COMPONENT_DEFINE(WakeWaveTrail, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Toggle, clear_button, 0, "", "", "Editor");
	PROP_PARAM(Toggle, save_mesh, 0, "", "", "Editor");
	PROP_PARAM(Int, num_quad, 0, "", "", "Editor");
	PROP_PARAM(Float, start_time, 0, "", "", "Editor");
	PROP_PARAM(Float, end_time, 0, "", "", "Editor");
	PROP_PARAM(Float, start_distance, 0, "", "", "Editor");
	PROP_PARAM(Float, end_distance, 0, "", "", "Editor");
	PROP_PARAM(Float, speed, 0, "", "", "Editor");

	PROP_PARAM(Node, mesh_decal_node, "", "", "Spawn", "filter=DecalMesh");
	PROP_PARAM(Float, width, 0.25f, "Wheel width", "Width of wheel in units(meters)", "Trail");
	PROP_PARAM(Float, max_distance_hide, 100.0f, "","","Trail");

	PROP_PARAM(String, num_quad_material_param_name, "num_quad", "","","Material");
	PROP_PARAM(String, start_time_material_param_name, "start_time", "","","Material");
	PROP_PARAM(String, end_time_material_param_name, "end_time", "","","Material");
	PROP_PARAM(String, start_distance_material_param_name, "start_distance", "","","Material");
	PROP_PARAM(String, end_distance_material_param_name, "end_distance", "","","Material");
	PROP_PARAM(String, max_distance_material_param_name, "max_distance", "","","Material");
	PROP_PARAM(String, width_material_param_name, "width", "","","Material");
	PROP_PARAM(String, speed_material_param_name, "speed", "","","Material");


	PROP_PARAM(Float, distance_eps, 1.0f, "","","Trail");
	PROP_PARAM(Float, direction_eps, 0.05f, "","","Trail");
	PROP_PARAM(Float, bound_increase, 10.0f, "","","Trail");
	PROP_PARAM(Toggle, debug, false, "","","Editor");

	PROP_PARAM(Int, count_segments, 2);

	void setEnabled(bool value);
	void clearButton();
	void saveMesh();

	void addPoint(const Vec3 &point);

private:
	void init();
	void update();
	void shutdown();

	void build_mesh();
	void hide();

	void add_point_internal(const Vec3 &point);
	void calculate_last_tangents();
	void draw_debug();

	MaterialPtr decal_mat;
	DecalMeshPtr decal;
	MeshPtr mesh;
	Mat4 itransform = Mat4_identity;

	Vec3 current_pos;
	Vec3 prev_pos;
	vec3 prev_frame_dir;
	Vector<PointInfo> path_points;
	bool need_remove_last = false;
};
