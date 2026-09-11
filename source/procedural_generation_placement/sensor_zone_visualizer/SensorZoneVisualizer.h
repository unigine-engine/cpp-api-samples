#pragma once

#include <UnigineComponentSystem.h>

// Procedural "sensor zone" mesh generator 
// : near/far are radial distances (spherical caps), the bounds are set
// by horizontal/vertical angles. Handles the full geometry, including fields of
// view greater than 180 degrees.
// Change the parameters and call refresh() to rebuild the mesh.
class SensorZoneVisualizer : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SensorZoneVisualizer, Unigine::ComponentBase);
	COMPONENT_INIT(init, 1);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Material, mat);
	PROP_PARAM(Toggle, change_color, false);
	PROP_PARAM(Color, mat_color, {0.f, 1.f, 1.f, 1.f});
	// Name of the material parameter driven by the color (see setColor).
	PROP_PARAM(String, color_parameter, "Emission Color");
	PROP_PARAM(Vec2, vertical_angles, Unigine::Math::vec2(-30.f, 30.f));
	PROP_PARAM(Vec2, horizontal_angles, Unigine::Math::vec2(-30.f, 30.f));
	PROP_PARAM(Int, num_stacks, 64);
	PROP_PARAM(Int, num_slices, 64);
	PROP_PARAM(Int, num_side_segments, 9);
	PROP_PARAM(Float, near, 0.5f);
	PROP_PARAM(Float, far, 2.0f);

	void refresh();
	void setColor(const Unigine::Math::vec4 &color);
	const Unigine::Math::vec4 &getColor() const;

private:
	struct SectorSurface
	{
		Unigine::Vector<Unigine::Math::vec3> vertices;
		Unigine::Vector<Unigine::Math::vec3> normals;
		Unigine::Vector<Unigine::Math::quat> tangents;
		int offset = 0;
	};

	void init();
	void update();
	void shutdown();

	void on_enable() override;
	void on_disable() override;

	void init_params();
	void update_mesh(Unigine::MeshPtr mesh);

	void add_sphere_data(SectorSurface &s, Unigine::Math::vec3 dir, bool is_pole);
	void create_sphere_vertex_data(SectorSurface &s, Unigine::Math::vec3 normal,
		Unigine::Math::vec3 up);
	void add_hsides_vertex_data(SectorSurface &s, Unigine::Math::vec3 dir, Unigine::Math::vec3 up);
	void add_vsides_vertex_data(SectorSurface &s, Unigine::Math::vec3 dir, Unigine::Math::vec3 up);

private:
	Unigine::MeshPtr mesh;
	Unigine::ObjectMeshStaticPtr obj;
	Unigine::MaterialPtr material = nullptr;
	Unigine::Math::vec4 current_color;
	int material_param_index = -1;

	int stacks = 64;
	int slices = 64;
	int deviders = 5;

	float near_dist = 0.5f;
	float far_dist = 2.0f;

	bool changed = false;

	float min_vertical_angle;
	float max_vertical_angle;

	float min_horizontal_angle;
	float max_horizontal_angle;
};
