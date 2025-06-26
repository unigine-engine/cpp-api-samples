#include "TriggerSample.h"

#include "MathTriggerComponent.h"

#include <UnigineConsole.h>
#include <UnigineVisualizer.h>


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


	// Setting callbacks

	world_trigger->getEventEnter().connect(trigger_connections, [this](const NodePtr &node_trigger) {
		ObjectMeshStaticPtr obj = checked_ptr_cast<ObjectMeshStatic>(postament_world.get());
		obj->setMaterial(postament_mat_triggered, 0);
	});

	world_trigger->getEventLeave().connect(trigger_connections, [this](const NodePtr &node_trigger) {
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
}

void TriggerSample::update()
{
	Visualizer::renderBoundBox(world_trigger->getBoundBox(),
		trigger_world_node.get()->getWorldTransform(), vec4_red);
}

void TriggerSample::shutdown()
{
	trigger_connections.disconnectAll();
	Visualizer::setMode(visualizer_mode);
	Visualizer::setEnabled(false);
}
