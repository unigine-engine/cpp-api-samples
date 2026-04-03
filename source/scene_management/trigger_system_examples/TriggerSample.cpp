// Comprehensive trigger demonstration. Shows PhysicalTrigger (physics bodies),
// WorldTrigger (nodes), WorldMathTrigger (custom point-in-bounds), WorldIntersectionTrigger
// (spatial queries), and NodeTrigger (enabled/position events). Each trigger type
// changes a postament material color when the target enters/leaves its bounds.

#include "TriggerSample.h"

#include "MathTriggerComponent.h"

#include <UnigineConsole.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(TriggerSample);


// Find intersections only with material ball
namespace
{
constexpr int MATERIAL_BALL_INTERSECTION_MASK = 0x7;
} // anonymous namespace

// All trigger types are configured and enter/leave callbacks are connected.
void TriggerSample::init()
{
	// Enable visualizer with depth testing for trigger bounds display
	Visualizer::setEnabled(true);
	visualizer_mode = Visualizer::getMode();
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);

	// Cast trigger nodes to PhysicalTrigger for physics-based detection
	physical_trigger_sphere = checked_ptr_cast<PhysicalTrigger>(trigger_physics_sphere_node.get());
	physical_trigger_capsule = checked_ptr_cast<PhysicalTrigger>(trigger_physics_capsule_node.get());
	physical_trigger_cylinder = checked_ptr_cast<PhysicalTrigger>(trigger_physics_cylinder_node.get());
	physical_trigger_box = checked_ptr_cast<PhysicalTrigger>(trigger_physics_box_node.get());

	// Get custom math-based trigger components
	WorldMathTrigger *math_trigger_box = getComponent<WorldMathTrigger>(
		trigger_math_box_node.get());
	WorldMathTrigger *math_trigger_sphere = getComponent<WorldMathTrigger>(
		trigger_math_sphere_node.get());
	world_trigger = checked_ptr_cast<WorldTrigger>(trigger_world_node.get());

	// Get intersection-based trigger components
	WorldIntersectionTrigger *world_intersections_trigger_sphere
		= getComponent<WorldIntersectionTrigger>(trigger_intersection_node_sphere.get());
	WorldIntersectionTrigger *world_intersectons_trigger_box
		= getComponent<WorldIntersectionTrigger>(trigger_intersection_node_box.get());

	// Register target object with math triggers for tracking
	math_trigger_box->addObject(target_to_check);
	math_trigger_sphere->addObject(target_to_check);

	NodeTriggerPtr node_trigger = checked_ptr_cast<NodeTrigger>(trigger_node_node.get());

	// PhysicalTrigger callbacks - detect physics bodies entering/leaving trigger shapes.
	// Each shape type (sphere, capsule, cylinder, box) has its own trigger and postament.

	// Sphere PhysicalTrigger: lights up sphere postament when body enters
	physical_trigger_sphere->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_sphere.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	// Sphere PhysicalTrigger: restores sphere postament when body leaves
	physical_trigger_sphere->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_sphere.get());
			obj->setMaterial(postament_mat, 0);
		});

	// Capsule PhysicalTrigger: lights up capsule postament when body enters
	physical_trigger_capsule->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_capsule.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	// Capsule PhysicalTrigger: restores capsule postament when body leaves
	physical_trigger_capsule->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_capsule.get());
			obj->setMaterial(postament_mat, 0);
		});

	// Cylinder PhysicalTrigger: lights up cylinder postament when body enters
	physical_trigger_cylinder->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_cylinder.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	// Cylinder PhysicalTrigger: restores cylinder postament when body leaves
	physical_trigger_cylinder->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_cylinder.get());
			obj->setMaterial(postament_mat, 0);
		});

	// Box PhysicalTrigger: lights up box postament when body enters
	physical_trigger_box->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_box.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	// Box PhysicalTrigger: restores box postament when body leaves
	physical_trigger_box->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_box.get());
			obj->setMaterial(postament_mat, 0);
		});

	// WorldTrigger callbacks - detect nodes entering/leaving axis-aligned box volume.
	// Unlike PhysicalTrigger, works with any node (not just physics bodies).

	// WorldTrigger: lights up world postament when node enters AABB
	world_trigger->getEventEnter().connect(*this, [this](const NodePtr &node_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_world.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	// WorldTrigger: restores world postament when node leaves AABB
	world_trigger->getEventLeave().connect(*this, [this](const NodePtr &node_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_world.get());
			obj->setMaterial(postament_mat, 0);
		});

	// WorldMathTrigger callbacks - custom component using point-inside-bounds checks.
	// Requires manual object registration via addObject(); checks position each frame.

	// Math sphere trigger: lights up math_sphere postament when registered node enters
	math_trigger_sphere->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_ENTER,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(
				postament_math_sphere.get());
			obj->setMaterial(postament_mat_triggered, 0);
		}));

	// Math sphere trigger: restores math_sphere postament when registered node leaves
	math_trigger_sphere->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_LEAVE,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(
				postament_math_sphere.get());
			obj->setMaterial(postament_mat, 0);
		}));

	// WorldIntersectionTrigger callbacks - uses World::getIntersection spatial queries.
	// Automatically detects any node in bounds; filters by intersection mask.

	// Intersection sphere trigger: lights up intersection_sphere postament for matching objects
	world_intersections_trigger_sphere->addCallback(
		WorldIntersectionTrigger::CALLBACK_TRIGGER_ENTER,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectPtr obj = checked_ptr_cast<Object>(node_trigger);
			if (obj && (obj->getIntersectionMask(0) == MATERIAL_BALL_INTERSECTION_MASK))
			{
				ObjectMeshStaticPtr postament = checked_ptr_cast<ObjectMeshStatic>(
					postament_intersection_sphere.get());
				postament->setMaterial(postament_mat_triggered, 0);
			}
		}));

	// Intersection sphere trigger: restores intersection_sphere postament when object leaves
	world_intersections_trigger_sphere->addCallback(
		WorldIntersectionTrigger::CALLBACK_TRIGGER_LEAVE,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectPtr obj = checked_ptr_cast<Object>(node_trigger);
			if (obj && (obj->getIntersectionMask(0) == MATERIAL_BALL_INTERSECTION_MASK))
			{
				ObjectMeshStaticPtr postament = checked_ptr_cast<ObjectMeshStatic>(
					postament_intersection_sphere.get());
				postament->setMaterial(postament_mat, 0);
			}
		}));

	// Math box trigger: lights up math_box postament when registered node enters
	math_trigger_box->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_ENTER,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_math_box.get());
			obj->setMaterial(postament_mat_triggered, 0);
		}));

	// Math box trigger: restores math_box postament when registered node leaves
	math_trigger_box->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_LEAVE,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_math_box.get());
			obj->setMaterial(postament_mat, 0);
		}));

	// Intersection box trigger: lights up intersection_box postament for matching objects
	world_intersectons_trigger_box->addCallback(WorldIntersectionTrigger::CALLBACK_TRIGGER_ENTER,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectPtr obj = checked_ptr_cast<Object>(node_trigger);
			if (obj && (obj->getIntersectionMask(0) == MATERIAL_BALL_INTERSECTION_MASK))
			{
				ObjectMeshStaticPtr postament = checked_ptr_cast<ObjectMeshStatic>(
					postament_intersection_box.get());
				postament->setMaterial(postament_mat_triggered, 0);
			}
		}));

	// Intersection box trigger: restores intersection_box postament when object leaves
	world_intersectons_trigger_box->addCallback(WorldIntersectionTrigger::CALLBACK_TRIGGER_LEAVE,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectPtr obj = checked_ptr_cast<Object>(node_trigger);
			if (obj && obj->getIntersectionMask(0) == MATERIAL_BALL_INTERSECTION_MASK)
			{
				ObjectMeshStaticPtr postament = checked_ptr_cast<ObjectMeshStatic>(
					postament_intersection_box.get());
				postament->setMaterial(postament_mat, 0);
			}
		}));

	// NodeTrigger callbacks - fires when node is enabled/disabled or moved

	// Updates text color based on trigger enabled state
	node_trigger->getEventEnabled().connect(*this, [this](const NodeTriggerPtr &trigger)
		{
			auto object_text = checked_ptr_cast<ObjectText>(trigger_node_text.get());
			if(trigger->isEnabled())
				object_text->setTextColor(vec4_white);
			else
				object_text->setTextColor(vec4_red);
		});

	// Animates parent material color when trigger position changes
	node_trigger->getEventPosition().connect(*this, [](const NodeTriggerPtr &trigger)
		{
			ObjectPtr parent = checked_ptr_cast<Object>(trigger->getParent());
			MaterialPtr material = parent->getMaterialInherit(0);
			vec4 color = material->getParameterFloat4("albedo_color");
			color.z += Game::getIFps();
			if (color.z > 1.0f)
				color.z = 0.0f;
			material->setParameterFloat4("albedo_color", color);
		});

	// UI setup

	sample_description_window.createWindow();
	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	auto node_trigger_checkbox = WidgetCheckBox::create("Cube Active");
	parameters->addChild(node_trigger_checkbox, Gui::ALIGN_LEFT);
	// Checkbox toggles the NodeTrigger's parent node enabled state
	node_trigger_checkbox->getEventChanged().connect(*this, [this, node_trigger_checkbox]()
		{
			trigger_node_parent_node.get()->setEnabled(node_trigger_checkbox->isChecked());
		});
	node_trigger_checkbox->setChecked(true);
}

// Trigger bounds are visualized each frame for debug visibility.
void TriggerSample::update()
{
	Visualizer::renderBoundBox(world_trigger->getBoundBox(),
		trigger_world_node.get()->getWorldTransform(), vec4_red);

	physical_trigger_sphere->renderVisualizer();
	physical_trigger_capsule->renderVisualizer();
	physical_trigger_cylinder->renderVisualizer();
	physical_trigger_box->renderVisualizer();
}

// Visualizer mode is restored and UI is cleaned up.
void TriggerSample::shutdown()
{
	Visualizer::setMode(visualizer_mode);
	Visualizer::setEnabled(false);

	sample_description_window.shutdown();
}
