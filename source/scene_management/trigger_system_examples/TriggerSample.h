// Comprehensive trigger demonstration showing multiple trigger types.
// Postaments change color when the target enters each trigger's bounds.
// Covers PhysicalTrigger, WorldTrigger, custom math triggers, and NodeTrigger.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>
#include <UnigineWorlds.h>
#include <UniginePhysicals.h>
#include <UnigineVisualizer.h>

#include "../../menu_ui/SampleDescriptionWindow.h"

// Sets up all trigger types and connects enter/leave callbacks to material changes.
class TriggerSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TriggerSample, ComponentBase);

	// The target object that enters trigger volumes
	PROP_PARAM(Node, target_to_check);
	// Visual indicators that change color when triggered
	PROP_PARAM(Node, postament_physics_sphere);
	PROP_PARAM(Node, postament_physics_capsule);
	PROP_PARAM(Node, postament_physics_cylinder);
	PROP_PARAM(Node, postament_physics_box);
	PROP_PARAM(Node, postament_world);
	PROP_PARAM(Node, postament_math_sphere);
	PROP_PARAM(Node, postament_math_box);
	PROP_PARAM(Node, postament_intersection_box);
	PROP_PARAM(Node, postament_intersection_sphere);

	// Trigger nodes for each trigger type demonstration
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

	// Materials swapped on postaments to indicate trigger state
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
	// WorldTrigger instance for node-based detection
	Unigine::WorldTriggerPtr world_trigger;

	// PhysicalTrigger instances for physics body detection
	Unigine::PhysicalTriggerPtr physical_trigger_sphere;
	Unigine::PhysicalTriggerPtr physical_trigger_capsule;
	Unigine::PhysicalTriggerPtr physical_trigger_cylinder;
	Unigine::PhysicalTriggerPtr physical_trigger_box;

	// Saved visualizer mode restored on shutdown
	Unigine::Visualizer::MODE visualizer_mode;

	SampleDescriptionWindow sample_description_window;
};
