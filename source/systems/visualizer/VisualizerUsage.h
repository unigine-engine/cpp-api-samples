#pragma once

#include <UnigineComponentSystem.h>

class VisualizerUsage : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(VisualizerUsage, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component displays information about the zoom sample and demonstrates "
		"how to use a simple zoom and how it affects player camera.");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	bool renderPoint2D = false;
	bool renderLine2D = false;
	bool renderTriangle2D = false;
	bool renderQuad2D = false;
	bool renderRectangle = false;
	bool renderMessage2D = false;

private:

	PROP_PARAM(Node, node_boundBox_example);
	PROP_PARAM(Node, node_boundSphere_example);

	PROP_PARAM(Node, node_object_example);
	PROP_PARAM(Node, node_object_solid_example);

	PROP_PARAM(Node, node_surface_example);
	PROP_PARAM(Node, node_surface_solid_example);

	PROP_PARAM(Node, node_object_surface_boundBox_example);
	PROP_PARAM(Node, node_object_surface_boundSphere_example);

	PROP_ARRAY(Node, postament_nodes);

	Unigine::ObjectPtr object_example;
	Unigine::ObjectPtr object_solid_example;

	Unigine::ObjectPtr surface_example;
	Unigine::ObjectPtr surface_solid_example;

	Unigine::ObjectPtr object_surface_boundBox_example;
	Unigine::ObjectPtr object_surface_boundSphere_example;

	



	void init();
	void update();
	void update3D();
	void update2D();
	Unigine::Math::Vec3 getPostamentPoint(int index);
};