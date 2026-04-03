#pragma once

#include <UnigineComponentSystem.h>

class VisualizerUsage : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(VisualizerUsage, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates how to use the Visualizer API to render various 2D and 3D "
		"debug primitives, such as points, lines, shapes, bounding volumes, and object surfaces.");


	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Toggle switches for 2D visualization elements
	bool renderPoint2D = false;
	bool renderLine2D = false;
	bool renderTriangle2D = false;
	bool renderQuad2D = false;
	bool renderRectangle = false;
	bool renderMessage2D = false;

private:
	// Nodes for visualization examples (set in the Editor)
	PROP_PARAM(Node, node_boundBox_example);
	PROP_PARAM(Node, node_boundSphere_example);

	PROP_PARAM(Node, node_object_example);
	PROP_PARAM(Node, node_object_solid_example);

	PROP_PARAM(Node, node_surface_example);
	PROP_PARAM(Node, node_surface_solid_example);

	PROP_PARAM(Node, node_object_surface_boundBox_example);
	PROP_PARAM(Node, node_object_surface_boundSphere_example);

	// Array of postament nodes for positioning
	PROP_ARRAY(Node, postament_nodes);

	// Object pointers for visualization targets
	Unigine::ObjectPtr object_example;
	Unigine::ObjectPtr object_solid_example;

	Unigine::ObjectPtr surface_example;
	Unigine::ObjectPtr surface_solid_example;

	Unigine::ObjectPtr object_surface_boundBox_example;
	Unigine::ObjectPtr object_surface_boundSphere_example;

	
	void init();
	void update();		// Main update loop
	void update3D();	// Update 3D visualization elements
	void update2D();	// Update 2D visualization elements

	// Get position from postament array
	Unigine::Math::Vec3 getPostamentPoint(int index);
};