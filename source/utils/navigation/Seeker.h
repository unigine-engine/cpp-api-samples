#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePathFinding.h>
#include "../../utils/Utils.h"

PROP_SWITCH_ENUM(ROUTE_TYPE, Route2D, Route3D);

class Seeker : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Seeker, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);
	
	PROP_PARAM(Node, target);
	PROP_PARAM(Float, movement_speed, 7.f);
	PROP_PARAM(Float, rotation_speed, 360.f);
	PROP_PARAM(Float, route_radius, 0.25f);
	PROP_PARAM(Float, route_height, 0.f);
	PROP_PARAM(Color, route_color, Unigine::Math::vec4_white);
	PROP_PARAM(Switch, route_type, Route2D, ROUTE_TYPE_STR);
	
	void setRouteRadius(float radius)
	{
		route_radius = radius;
		route->setRadius(radius);
	}
	
private:
	
	void init();
	void update();
	void shutdown();
	void calculate_route();
	bool is_inside_navigation();
	
	Unigine::PathRoutePtr route;
};
