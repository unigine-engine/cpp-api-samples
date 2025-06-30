#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
#include <UnigineWorlds.h>
#include <UniginePhysicals.h>
#include <UnigineVisualizer.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

class TriggerSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TriggerSample, ComponentBase);

	PROP_PARAM(Node, target_to_check);
	PROP_PARAM(Node, postament_physics_sphere);
	PROP_PARAM(Node, postament_physics_capsule);
	PROP_PARAM(Node, postament_physics_cylinder);
	PROP_PARAM(Node, postament_physics_box);
	PROP_PARAM(Node, postament_world);
	PROP_PARAM(Node, postament_math_sphere);
	PROP_PARAM(Node, postament_math_box);
	PROP_PARAM(Node, postament_intersection_box);
	PROP_PARAM(Node, postament_intersection_sphere);

	PROP_PARAM(Node, trigger_physics_sphere_node);
	PROP_PARAM(Node, trigger_physics_capsule_node);
	PROP_PARAM(Node, trigger_physics_cylinder_node);
	PROP_PARAM(Node, trigger_physics_box_node);
	PROP_PARAM(Node, trigger_world_node);
	PROP_PARAM(Node, trigger_math_box_node);
	PROP_PARAM(Node, trigger_math_sphere_node);
	PROP_PARAM(Node, trigger_intersection_node_box);
	PROP_PARAM(Node, trigger_intersection_node_sphere);
	PROP_PARAM(Node, trigger_node_node);
	PROP_PARAM(Node, trigger_node_parent_node);
	PROP_PARAM(Node, trigger_node_text);

	PROP_PARAM(Material, postament_mat);
	PROP_PARAM(Material, postament_mat_triggered);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

private:
	void init();
	void update();
	void shutdown();

private:
	Unigine::WorldTriggerPtr world_trigger;

	Unigine::PhysicalTriggerPtr physical_trigger_sphere;
	Unigine::PhysicalTriggerPtr physical_trigger_capsule;
	Unigine::PhysicalTriggerPtr physical_trigger_cylinder;
	Unigine::PhysicalTriggerPtr physical_trigger_box;

	Unigine::Visualizer::MODE visualizer_mode;

	SampleDescriptionWindow sample_description_window;
};
