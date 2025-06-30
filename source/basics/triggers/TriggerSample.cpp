#include "TriggerSample.h"

#include "MathTriggerComponent.h"

#include <UnigineConsole.h>
#include <UnigineVisualizer.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(TriggerSample);


// find intersections only with material ball
namespace
{
constexpr int MATERIAL_BALL_INTERSECTION_MASK = 0x7;
} // anonymous namespace

void TriggerSample::init()
{
	Visualizer::setEnabled(true);
	visualizer_mode = Visualizer::getMode();
	Visualizer::setMode(Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED);

	physical_trigger_sphere = checked_ptr_cast<PhysicalTrigger>(trigger_physics_sphere_node.get());
	physical_trigger_capsule = checked_ptr_cast<PhysicalTrigger>(trigger_physics_capsule_node.get());
	physical_trigger_cylinder = checked_ptr_cast<PhysicalTrigger>(trigger_physics_cylinder_node.get());
	physical_trigger_box = checked_ptr_cast<PhysicalTrigger>(trigger_physics_box_node.get());

	WorldMathTrigger *math_trigger_box = getComponent<WorldMathTrigger>(
		trigger_math_box_node.get());
	WorldMathTrigger *math_trigger_sphere = getComponent<WorldMathTrigger>(
		trigger_math_sphere_node.get());
	world_trigger = checked_ptr_cast<WorldTrigger>(trigger_world_node.get());
	WorldIntersectionTrigger *world_intersections_trigger_sphere
		= getComponent<WorldIntersectionTrigger>(trigger_intersection_node_sphere.get());
	WorldIntersectionTrigger *world_intersectons_trigger_box
		= getComponent<WorldIntersectionTrigger>(trigger_intersection_node_box.get());
	math_trigger_box->addObject(target_to_check);
	math_trigger_sphere->addObject(target_to_check);

	NodeTriggerPtr node_trigger = checked_ptr_cast<NodeTrigger>(trigger_node_node.get());

	// Setting callbacks

	physical_trigger_sphere->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_sphere.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	physical_trigger_sphere->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_sphere.get());
			obj->setMaterial(postament_mat, 0);
		});

	physical_trigger_capsule->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_capsule.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	physical_trigger_capsule->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_capsule.get());
			obj->setMaterial(postament_mat, 0);
		});

	physical_trigger_cylinder->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_cylinder.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	physical_trigger_cylinder->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_cylinder.get());
			obj->setMaterial(postament_mat, 0);
		});

	physical_trigger_box->getEventEnter().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_box.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	physical_trigger_box->getEventLeave().connect(*this, [this](const BodyPtr &body_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_physics_box.get());
			obj->setMaterial(postament_mat, 0);
		});

	world_trigger->getEventEnter().connect(*this, [this](const NodePtr &node_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_world.get());
			obj->setMaterial(postament_mat_triggered, 0);
		});

	world_trigger->getEventLeave().connect(*this, [this](const NodePtr &node_trigger)
		{
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_world.get());
			obj->setMaterial(postament_mat, 0);
		});


	math_trigger_sphere->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_ENTER,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(
				postament_math_sphere.get());
			obj->setMaterial(postament_mat_triggered, 0);
		}));

	math_trigger_sphere->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_LEAVE,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(
				postament_math_sphere.get());
			obj->setMaterial(postament_mat, 0);
		}));


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


	math_trigger_box->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_ENTER,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_math_box.get());
			obj->setMaterial(postament_mat_triggered, 0);
		}));

	math_trigger_box->addCallback(WorldMathTrigger::CALLBACK_TRIGGER_LEAVE,
		MakeCallback([this](const NodePtr &node_trigger) {
			ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_math_box.get());
			obj->setMaterial(postament_mat, 0);
		}));


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

	node_trigger->getEventEnabled().connect(*this, [this](const NodeTriggerPtr &trigger)
		{
			auto object_text = checked_ptr_cast<ObjectText>(trigger_node_text.get());
			if(trigger->isEnabled())
				object_text->setTextColor(vec4_white);
			else
				object_text->setTextColor(vec4_red);
		});

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

	sample_description_window.createWindow();
	WidgetGroupBoxPtr parameters = sample_description_window.getParameterGroupBox();
	auto node_trigger_checkbox = WidgetCheckBox::create("Cube Active");
	parameters->addChild(node_trigger_checkbox, Gui::ALIGN_LEFT);
	node_trigger_checkbox->getEventChanged().connect(*this, [this, node_trigger_checkbox]()
		{
			trigger_node_parent_node.get()->setEnabled(node_trigger_checkbox->isChecked());
		});
	node_trigger_checkbox->setChecked(true);
}

void TriggerSample::update()
{
	Visualizer::renderBoundBox(world_trigger->getBoundBox(),
		trigger_world_node.get()->getWorldTransform(), vec4_red);

	physical_trigger_sphere->renderVisualizer();
	physical_trigger_capsule->renderVisualizer();
	physical_trigger_cylinder->renderVisualizer();
	physical_trigger_box->renderVisualizer();
}

void TriggerSample::shutdown()
{
	Visualizer::setMode(visualizer_mode);
	Visualizer::setEnabled(false);

	sample_description_window.shutdown();
}
