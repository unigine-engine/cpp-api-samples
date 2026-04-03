// Demonstrates spatial intersection queries using frustum, sphere, and box
// bounds. Objects intersecting each bound have their material changed.
// Uses WorldBound types for double-precision compatibility.

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

// Bound volumes are constructed from component parameters; visualizer is enabled.
void BoundIntersection::init()
{
	Visualizer::setEnabled(true);

	// Build frustum from look-at view matrix and perspective projection
	frustum_view_matrix =
		Mat4(lookAt(bound_frustum->eye_position, bound_frustum->target_position, bound_frustum->up_direction, bound_frustum->forward_axis));
	float view_aspect = static_cast<float>(bound_frustum->view_width) / bound_frustum->view_height;
	frustum_projection_matrix =
		perspective(bound_frustum->view_fov, view_aspect, bound_frustum->view_distance_min, bound_frustum->view_distance_max);

	// WorldBound types ensure correct behavior in double-precision builds
	frustum = WorldBoundFrustum(frustum_projection_matrix, frustum_view_matrix);
	sphere = WorldBoundSphere(Vec3(bound_sphere->position), bound_sphere->radius);
	box = WorldBoundBox(Vec3(bound_box->minimum), Vec3(bound_box->maximum));

	init_ui();
	// Get rotator reference for rotation speed UI control
	rotator_component = ComponentSystem::get()->getComponentInWorld<Rotator>();
}

// Each bound type is tested for intersections; matching objects are highlighted.
void BoundIntersection::update()
{
	// Reused for each intersection query (cleared automatically by getIntersection)
	Vector<NodePtr> found;

	// Test frustum intersection and apply green material
	World::getIntersection(frustum, found);
	change_color(found, bound_frustum->intersection_material);

	// Test sphere intersection and apply red material
	World::getIntersection(sphere, found);
	change_color(found, bound_sphere->intersection_material);

	// Test box intersection and apply blue material
	World::getIntersection(box, found);
	change_color(found, bound_box->intersection_material);

	render_all_bounds();
}

// UI window is closed; visualizer is disabled.
void BoundIntersection::shutdown()
{
	window.shutdown();
	Visualizer::setEnabled(false);
}

// Original materials are restored at end of frame so highlights only last one frame.
void BoundIntersection::swap()
{
	for (auto &it : changed)
	{
		it.key->setMaterial(it.data, 0);
	}
	changed.clear();
}

// Intersected objects have their material temporarily replaced with highlight color.
void BoundIntersection::change_color(const Vector<NodePtr> &found, const MaterialPtr &new_material)
{
	for (int i = 0, found_size = found.size(); i < found_size; ++i)
	{
		if (found[i].isDeleted())
			continue;

		// Only process mesh objects (skip lights, players, etc.)
		if (ObjectMeshStaticPtr mesh = checked_ptr_cast<ObjectMeshStatic>(found[i]))
		{
			// Store original material for restoration in swap()
			changed.append(mesh, mesh->getMaterial(0));
			mesh->setMaterial(new_material, 0);
		}
	}
}

// Debug visualization of all bound volumes and object bounding boxes.
void BoundIntersection::render_all_bounds() const
{
	// Draw test volumes in their respective highlight colors
	Visualizer::renderFrustum(frustum_projection_matrix, inverse(frustum_view_matrix), vec4_green);
	Visualizer::renderBoundSphere(sphere, Mat4_identity, vec4_red);
	Visualizer::renderBoundBox(box, Mat4_identity, vec4_blue);

	// Draw bounding boxes of all test primitives
	NodePtr primitives = World::getNodeByName("primitives");
	if (!primitives)
		return;

	for (int i = 0; i < primitives->getNumChildren(); ++i)
		Visualizer::renderNodeBoundBox(primitives->getChild(i), vec4_white);
}

// UI window with rotation speed slider is created.
void BoundIntersection::init_ui()
{
	window.createWindow();

	auto parameters = window.getParameterGroupBox();
	WidgetVBoxPtr vbox = WidgetVBox::create();
	window.addFloatParameter("Rotation Speed", "Adjusts the rotation speed of the figures", 25.0f, 0.0f, 50.0f,
		[this](float new_z) { rotator_component->angular_velocity = vec3(0, 0, new_z); });
}
