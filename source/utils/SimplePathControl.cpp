#include "SimplePathControl.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>

using namespace Unigine;
using namespace Math;

void recursiveCall(const Unigine::NodePtr &node, std::function<void(const Unigine::NodePtr &)> func)
{
	if (!node)
	{
		return;
	}

	func(node);

	if (node->getType() == Unigine::Node::NODE_REFERENCE)
	{
		auto ref = Unigine::checked_ptr_cast<Unigine::NodeReference>(node);
		if (ref && ref->getReference())
		{
			recursiveCall(ref->getReference(), func);
		}
	}

	for (int i = 0; i < node->getNumChildren(); i++)
	{
		recursiveCall(node->getChild(i), func);
	}
}

REGISTER_COMPONENT(SimplePathControl);

void SimplePathControl::init()
{
	float min_distance = Consts::INF;
	if (path_node.get())
	{
		for (int i = 0; i < path_node->getNumChildren(); i++)
		{
			auto p = path_node->getChild(i)->getWorldTransform();
			float distance = (float)length2(node->getWorldPosition() - p.getTranslate());
			if (distance < min_distance)
			{
				current_path_index = i;
				min_distance = distance;
			}
			path.append(p);
		}
	}
}

void SimplePathControl::update()
{
	if (!enable_button.get())
	{
		return;
	}
	if (init_button)
	{
		path.clear();
		init();
		init_button = 0;
	}

	if (path.empty())
	{
		return;
	}

	if (Game::getTime() < wait_before_start)
	{
		return;
	}

	auto current_position = node->getWorldPosition();
	auto current_rot = node->getWorldRotation();


	auto route = path[current_path_index];

	float ifps = Game::getIFps();
	if (ifps == 0)
	{
		return;
	}

	vec3 ddir = vec3(route.getTranslate() - current_position).normalizeValid();
	Visualizer::renderLine3D(current_position, route.getTranslate(), vec4_red);

	float d = dot(current_rot.getMat3().getAxisX(), ddir);
	if (isnan(d))
	{
		d = 0;
	}
	rot_acceleration += (d > rot_acceleration ? ifps : -ifps);

	current_rot = current_rot * quat(vec3_up, torque_param * ifps * -rot_acceleration);

	current_position = current_position + Vec3(current_rot.getMat3().getAxisY()) * ifps * speed;

	node->setWorldPosition(current_position);
	node->setWorldRotation(current_rot);

	float distance_to_target = (float)length(route.getTranslate() - current_position);

	if (distance_to_target < 10)
	{
		current_path_index += dir;

		if (current_path_index == path.size())
		{
			if (circle)
			{
				current_path_index = 0;
			}
			else
			{
				dir *= -1;
				current_path_index += dir;
			}
		}
		else if (current_path_index == -1)
		{
			if (circle)
			{
				current_path_index = path.size() - 1;
			}
			else
			{
				dir *= -1;
				current_path_index += dir;
			}
		}
	}
}

void SimplePathControl::shutdown() {}
