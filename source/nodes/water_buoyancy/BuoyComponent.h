#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>

class BuoyComponent : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(BuoyComponent, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component allows to impart buoyancy without physical simulation.");
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	PROP_PARAM(Float, mass, 1.0f)

	PROP_PARAM(Node, point_front_center)
	PROP_PARAM(Node, point_back_left)
	PROP_PARAM(Node, point_back_right)

private:
	void init();
	void update();
	Unigine::ObjectWaterGlobalPtr water;
};