#pragma once

#include "UnigineMaterial.h"
#include "UnigineObjects.h"
#include "UnigineVector.h"
#include <UnigineComponentSystem.h>
#include "../../menu_ui/SampleDescriptionWindow.h"

class Rotator;

class BoundIntersection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BoundIntersection, Unigine::ComponentBase)
	COMPONENT_INIT(init)
	COMPONENT_UPDATE(update)
	COMPONENT_SHUTDOWN(shutdown)
	COMPONENT_SWAP(swap)

	struct SphereStruct : public Unigine::ComponentStruct
	{
		PROP_PARAM(Vec3, position, Unigine::Math::vec3_zero);
		PROP_PARAM(Float, radius, 1.0f);
		PROP_PARAM(Material, intersection_material);
	};
	struct FrustumStruct : public Unigine::ComponentStruct
	{
		PROP_PARAM(Vec3, eye_position, Unigine::Math::vec3_zero);
		PROP_PARAM(Vec3, target_position, Unigine::Math::vec3_forward);
		PROP_PARAM(Vec3, up_direction, Unigine::Math::vec3_up);
		PROP_PARAM(Int, forward_axis, Unigine::Math::AXIS_NZ);
		PROP_PARAM(Float, view_fov, 60.0f);
		PROP_PARAM(Int, view_width, 16);
		PROP_PARAM(Int, view_height, 9);
		PROP_PARAM(Float, view_distance_min, 0.1f);
		PROP_PARAM(Float, view_distance_max, 10.0f);
		PROP_PARAM(Material, intersection_material);
	};
	struct BoxStruct : public Unigine::ComponentStruct
	{
		PROP_PARAM(Vec3, minimum, Unigine::Math::vec3_zero);
		PROP_PARAM(Vec3, maximum, Unigine::Math::vec3_zero);
		PROP_PARAM(Material, intersection_material);
	};

	PROP_STRUCT(SphereStruct, bound_sphere, "Bound Sphere");
	PROP_STRUCT(FrustumStruct, bound_frustum, "Bound Frustum");
	PROP_STRUCT(BoxStruct, bound_box, "Bound Box");

private:
	// Using WorldBound types to support both single- and double-precision builds.
	Unigine::Math::WorldBoundFrustum frustum;
	Unigine::Math::Mat4 frustum_view_matrix;
	Unigine::Math::mat4 frustum_projection_matrix;
	Unigine::Math::WorldBoundBox box;
	Unigine::Math::WorldBoundSphere sphere;

	Unigine::Map<Unigine::ObjectMeshStaticPtr, Unigine::MaterialPtr> changed;
	void change_color(const Unigine::Vector<Unigine::NodePtr> &nodes, const Unigine::MaterialPtr &new_material);
	void render_all_bounds() const;

	Rotator *rotator_component;

	SampleDescriptionWindow window;

	void init_ui();

	void init();
	void update();
	void shutdown();
	void swap();
};