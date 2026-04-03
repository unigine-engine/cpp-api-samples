// Custom trigger components using math-based bounds checking. WorldMathTrigger
// tests registered objects against a sphere or box bound each frame using point-inside
// checks. WorldIntersectionTrigger uses World::getIntersection for spatial queries
// to detect any nodes entering/leaving the volume.

#include "MathTriggerComponent.h"

#include <UnigineConsole.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(WorldMathTrigger);

using namespace Unigine;
using namespace Math;

// Sphere and box bounds are initialized at component's world position.
void WorldMathTrigger::init()
{
	Vec3 translation = node->getWorldTransform().getTranslate();
	Vec3 coordinates_for_bound_box_min = -Vec3(bound_box_size / 2.0f);
	Vec3 coordinates_for_bound_box_max = Vec3(bound_box_size / 2.0f);
	bound_box.set(vec3(coordinates_for_bound_box_min), vec3(coordinates_for_bound_box_max));
	bound_box.setTransform(node->getWorldTransform());

	bound_sphere.set(vec3(translation), bound_sphere_size.get() / 2.0f);
	bound_sphere.setTransform(node->getWorldTransform());
}


// Bounds are repositioned and all objects are checked for enter/leave events.
void WorldMathTrigger::update()
{
	replace_bounds();

	if (debug)
	{
		visualize_bounds();
	}
	check_entered();
	check_leave();
}


// Objects that left the volume are removed from entered list and callback is fired.
void WorldMathTrigger::check_leave()
{
	for (const auto &obj : objects)
	{
		if (!entered.contains(obj))
			continue;
		if (obj.isDeleted())
		{
			entered.removeOneFast(obj);
			continue;
		}
		bool is_inside = is_sphere ? check_sphere(obj) : check_box(obj);
		if (!is_inside)
		{
			entered.removeOneFast(obj);
			if (is_callback(CALLBACK_TRIGGER_LEAVE))
				run_callback(CALLBACK_TRIGGER_LEAVE, obj);
		}
	}
}

// Bounds are repositioned to follow the component's node.
void WorldMathTrigger::replace_bounds()
{
	Vec3 translation = node->getWorldTransform().getTranslate();
	Vec3 coordinates_for_bound_box_min = -Vec3(bound_box_size / 2.0f) + translation;
	Vec3 coordinates_for_bound_box_max = Vec3(bound_box_size / 2.0f) + translation;
	bound_box.set(vec3(coordinates_for_bound_box_min), vec3(coordinates_for_bound_box_max));
	bound_sphere.set(vec3(translation), bound_sphere_size.get() / 2.0f);
}

// Debug wireframe is rendered for the active bound shape.
void WorldMathTrigger::visualize_bounds()
{
	if (is_sphere)
		Visualizer::renderSphere(bound_sphere_size / 2.0f, node->getWorldTransform(), vec4_red);
	else
		Visualizer::renderBoundBox(bound_box, Mat4_identity, vec4_red);
}


// Objects newly inside the volume are added to entered list and callback is fired.
void WorldMathTrigger::check_entered()
{
	for (const auto &obj : objects)
	{
		if (entered.contains(obj))
			continue;
		bool is_inside = is_sphere ? check_sphere(obj) : check_box(obj);
		if (is_inside)
		{
			entered.push_back(obj);
			if (is_callback(CALLBACK_TRIGGER_ENTER))
				run_callback(CALLBACK_TRIGGER_ENTER, obj);
		}
	}
}


// Tests if object's position is inside the box bounds.
inline bool WorldMathTrigger::check_box(const NodePtr &obj) const
{
	return bound_box.insideValid(vec3(obj->getTransform().getTranslate()));
}


// Tests if object's position is inside the sphere bounds.
inline bool WorldMathTrigger::check_sphere(const NodePtr &obj) const
{
	return bound_sphere.insideValid(vec3(obj->getTransform().getTranslate()));
}


void *WorldMathTrigger::addCallback(CALLBACK_TRIGGER callback, CallbackBase1<NodePtr> *func)
{
	return signal[callback].add(func);
}


bool WorldMathTrigger::removeCallback(CALLBACK_TRIGGER callback, void *id)
{
	return signal[callback].remove(id);
}


void WorldMathTrigger::clearCallbacks(CALLBACK_TRIGGER callback)
{
	signal[callback].clear();
}


inline bool WorldMathTrigger::is_callback(CALLBACK_TRIGGER callback) const
{
	return signal[callback].size() > 0 ? true : false;
}


inline void WorldMathTrigger::run_callback(CALLBACK_TRIGGER callback, const NodePtr &obj)
{
	signal[callback].invoke(obj);
}


// All registered callbacks are cleared on component destruction.
void WorldMathTrigger::shutdown()
{
	for (int i = 0; i < CALLBACK_TRIGGER_NUM; i++)
	{
		signal[i].clear();
	}
}

void WorldMathTrigger::addObject(const Unigine::NodePtr &obj)
{
	objects.push_back(obj);
}

void WorldMathTrigger::addObjects(const Unigine::Vector<Unigine::NodePtr> &input_objects)
{
	objects.append(input_objects);
	;
}

void WorldMathTrigger::removeObject(const Unigine::NodePtr &obj)
{
	auto it = objects.find(obj);
	if (it != objects.end())
		objects.remove(it);
}


// World Intersection Trigger Component

REGISTER_COMPONENT(WorldIntersectionTrigger);

// Sphere and box bounds are initialized at component's world position.
void WorldIntersectionTrigger::init()
{
	Vec3 translation = node->getWorldTransform().getTranslate();
	Vec3 coordinates_for_bound_box_min = -Vec3(bound_box_size / 2.0f) + translation;
	Vec3 coordinates_for_bound_box_max = Vec3(bound_box_size / 2.0f) + translation;
	bound_box.set(vec3(coordinates_for_bound_box_min), vec3(coordinates_for_bound_box_max));
	bound_sphere.set(vec3(translation), bound_sphere_size.get() / 2.0f);
}

// World intersection is queried and enter/leave callbacks are fired.
void WorldIntersectionTrigger::update()
{
	replace_bounds();

	if (debug)
	{
		visualize_bounds();
	}

	get_inside_nodes();
	check_entered();
	check_leave();
}

// All registered callbacks are cleared on component destruction.
void WorldIntersectionTrigger::shutdown()
{
	for (int i = 0; i < CALLBACK_TRIGGER_NUM; i++)
	{
		signal[i].clear();
	}
}

// Nodes found in current query but not in entered list trigger enter callback.
void WorldIntersectionTrigger::check_entered()
{
	for (const auto &it : inside)
	{
		if (entered.find(it) == entered.end())
		{
			entered.push_back(it);
			run_callback(CALLBACK_TRIGGER_ENTER, it);
		}
	}
}

// Entered nodes not in current query are removed and leave callback is fired.
void WorldIntersectionTrigger::check_leave()
{
	// Reverse iteration: removeOneFast swaps with last element, safe when going backwards
	for (int i = entered.size() - 1; i >= 0; i--)
	{
		if (entered[i].isDeleted())
		{
			entered.removeFast(i);
			continue;
		}

		if (inside.find(entered[i]) == inside.end())
		{
			run_callback(CALLBACK_TRIGGER_LEAVE, entered[i]);
			entered.removeFast(i);
		}
	}
}

// Bounds are repositioned to follow the component's node.
void WorldIntersectionTrigger::replace_bounds()
{
	Vec3 translation = node->getWorldTransform().getTranslate();
	Vec3 coordinates_for_bound_box_min = -Vec3(bound_box_size / 2.0f) + translation;
	Vec3 coordinates_for_bound_box_max = Vec3(bound_box_size / 2.0f) + translation;
	bound_box.set(vec3(coordinates_for_bound_box_min), vec3(coordinates_for_bound_box_max));
	bound_sphere.set(vec3(translation), bound_sphere_size.get() / 2.0f);
}

// Debug wireframe is rendered for the active bound shape.
void WorldIntersectionTrigger::visualize_bounds()
{
	if (is_sphere)
		Visualizer::renderSphere(bound_sphere_size / 2.0f, node->getWorldTransform(), vec4_red);
	else
		Visualizer::renderBoundBox(bound_box, Mat4_identity, vec4_red);
}

// World intersection query returns all nodes currently inside the bounds.
void WorldIntersectionTrigger::get_inside_nodes()
{
	if (is_sphere)
		World::getIntersection(bound_sphere, inside);
	else
		World::getIntersection(bound_box, inside);
}


void *WorldIntersectionTrigger::addCallback(CALLBACK_TRIGGER callback, CallbackBase1<NodePtr> *func)
{
	return signal[callback].add(func);
}


bool WorldIntersectionTrigger::removeCallback(CALLBACK_TRIGGER callback, void *id)
{
	return signal[callback].remove(id);
}


void WorldIntersectionTrigger::clearCallbacks(CALLBACK_TRIGGER callback)
{
	signal[callback].clear();
}


inline bool WorldIntersectionTrigger::is_callback(CALLBACK_TRIGGER callback) const
{
	return signal[callback].size() > 0 ? true : false;
}


inline void WorldIntersectionTrigger::run_callback(CALLBACK_TRIGGER callback, const NodePtr &obj)
{
	signal[callback].invoke(obj);
}
