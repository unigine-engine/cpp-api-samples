#include "BoundIntersection.h"
#include "UnigineComponentSystem.h"
#include "UnigineMaterial.h"
#include "UnigineMathLib.h"
#include "UnigineMathLibBounds.h"
#include "UnigineMathLibMat4.h"
#include "UnigineMathLibVec4.h"
#include "UnigineObjects.h"
#include "UniginePtr.h"
#include "UnigineVisualizer.h"
#include "UnigineWidgets.h"
#include "../../utils/Rotator.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(BoundIntersection)

void BoundIntersection::init()
{
	Visualizer::setEnabled(true);
	// initializing bound objects
	frustum_view_matrix =
		Mat4(lookAt(bound_frustum->eye_position, bound_frustum->target_position, bound_frustum->up_direction, bound_frustum->forward_axis));
	float view_aspect = static_cast<float>(bound_frustum->view_width) / bound_frustum->view_height;
	frustum_projection_matrix =
		perspective(bound_frustum->view_fov, view_aspect, bound_frustum->view_distance_min, bound_frustum->view_distance_max);

	// Using WorldBound types to support both single- and double-precision builds.
	// For float builds, regular Bound types can be used.
	frustum = WorldBoundFrustum(frustum_projection_matrix, frustum_view_matrix);
	sphere = WorldBoundSphere(Vec3(bound_sphere->position), bound_sphere->radius);
	box = WorldBoundBox(Vec3(bound_box->minimum), Vec3(bound_box->maximum));

	init_ui();
	rotator_component = ComponentSystem::get()->getComponentInWorld<Rotator>();
}

void BoundIntersection::update()
{
	// Vector of nodes where intersected ones will be stored.
	Vector<NodePtr> found;

	// The function World::getIntersection() clears the passed vector automatically,
	// so there's no need to clear it manually.
	World::getIntersection(frustum, found);
	change_color(found, bound_frustum->intersection_material);

	World::getIntersection(sphere, found);
	change_color(found, bound_sphere->intersection_material);

	World::getIntersection(box, found);
	change_color(found, bound_box->intersection_material);

	render_all_bounds();
}

void BoundIntersection::shutdown()
{
	window.shutdown();
	Visualizer::setEnabled(false);
}

void BoundIntersection::swap()
{
	for (auto &it : changed)
	{
		it.key->setMaterial(it.data, 0);
	}
	changed.clear();
}

// This function is used for demonstration purposes.
// It saves the previous material to the "changed" map
// and changes the material of all nodes of type ObjectMeshStatic.
void BoundIntersection::change_color(const Vector<NodePtr> &found, const MaterialPtr &new_material)
{
	for (int i = 0, found_size = found.size(); i < found_size; ++i)
	{
		if (found[i].isDeleted())
			continue;

		if (ObjectMeshStaticPtr mesh = checked_ptr_cast<ObjectMeshStatic>(found[i]))
		{
			changed.append(mesh, mesh->getMaterial(0));
			mesh->setMaterial(new_material, 0);
		}
	}
}

void BoundIntersection::render_all_bounds() const
{
	Visualizer::renderFrustum(frustum_projection_matrix, inverse(frustum_view_matrix), vec4_green);
	Visualizer::renderBoundSphere(sphere, Mat4_identity, vec4_red);
	Visualizer::renderBoundBox(box, Mat4_identity, vec4_blue);

	NodePtr primitives = World::getNodeByName("primitives");

	for (int i = 0; i < primitives->getNumChildren(); ++i)
	{
		if (primitives->getChild(i).isDeleted())
			return;

		Visualizer::renderNodeBoundBox(primitives->getChild(i), vec4_white);
	}
}

void BoundIntersection::init_ui()
{
	window.createWindow();

	auto parameters = window.getParameterGroupBox();
	WidgetVBoxPtr vbox = WidgetVBox::create();
	window.addFloatParameter("Rotation Speed", "Adjusts the rotation speed of the figures", 25.0f, 0.0f, 50.0f,
		[this](float new_z) { rotator_component->angular_velocity = vec3(0, 0, new_z); });
}
