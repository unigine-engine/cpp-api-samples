#include "Seeker.h"

#include <UnigineVisualizer.h>
#include <UnigineGame.h>

#include "Target.h"

REGISTER_COMPONENT(Seeker);

using namespace Unigine;
using namespace Unigine::Math;

void Seeker::init()
{
	Visualizer::setEnabled(true);
	
	route = PathRoute::create();
	route->setRadius(route_radius);
	route->setHeight(route_height);
}

void Seeker::update()
{
	if (!target)
		return;
	
	float ifps = Game::getIFps();
	
	if (route->isReached())
	{
		vec3 direction { route->getPoint(1) - route->getPoint(0) };
		auto target_component = getComponent<PathfindingTarget>(target);
		float target_radius = 0.5f;
		
		if (target_component)
			target_radius = target_component->radius;
			
		if (distance(node->getWorldPosition(), target->getWorldPosition()) < target_radius)
		{
			if (target_component)
				target_component->onReached();
		}
		
		else
		{
			auto last_valid_position = node->getWorldPosition();
			auto current_rotation = node->getWorldRotation();
			auto target_rotation = current_rotation;
			
			if (direction.length2() > FLT_EPSILON)
				target_rotation = rotationFromDir(direction);
			
			float movement_speed_factor = 1.f - getAngle(current_rotation, target_rotation) * (1.f / 180.f);
			
			node->setWorldRotation(rotateTowards(current_rotation, target_rotation, rotation_speed * ifps), true);
			node->translate(Vec3_forward * movement_speed * movement_speed_factor * ifps);
			
			calculate_route();
			
			if (!is_inside_navigation())
				node->setWorldPosition(last_valid_position);
		}
		
		route->renderVisualizer(route_color);
	}
	
	else
	{
		Visualizer::renderLine3D(node->getWorldPosition(), target->getWorldPosition(), vec4_red, ifps);
		calculate_route();
	}
}

bool Seeker::is_inside_navigation()
{
	for (int i = 0; i < route->getNumPoints(); i += 1)
	{
		NavigationPtr navigation = route->getNavigation(i);
		bool is_inside = false;
		
		switch (route_type.get())
		{
			case Route2D:
				is_inside = navigation->inside2D(node->getWorldPosition(), route_radius);
				break;
			
			case Route3D:
				is_inside = navigation->inside3D(node->getWorldPosition(), route_radius);
				break;
		}
		
		if (is_inside)
			return true;
	}
	
	return false;
}

void Seeker::calculate_route()
{
	switch (route_type)
	{
		case Route2D:
			route->create2D(node->getWorldPosition(), target->getWorldPosition());
			break;
		
		case Route3D:
			route->create3D(node->getWorldPosition(), target->getWorldPosition());
			break;
	}
}

void Seeker::shutdown()
{
	Visualizer::setEnabled(false);
}
