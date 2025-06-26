#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

class Selection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Selection, Unigine::ComponentBase);

	COMPONENT_UPDATE(update);

	Unigine::Math::Vec3 getCenter();
	float getBoundRadius();

	bool hasSelection();

protected:
	void update();

private:
	void updateBoundShpere();

	bool is_selection = false;
	// selection start position in window space
	Unigine::Math::ivec2 selection_start_mouse_position;
	// normalized selection start position in [0, 1] range
	Unigine::Math::vec2 upper_left_selection_corner;
	// normalized selection end position in [0, 1] range
	Unigine::Math::vec2 bottom_right_selection_corner;

	// frustum for selection box
	Unigine::Math::WorldBoundFrustum frustum;
	// vector of selectable objects in selection box
	Unigine::Vector<Unigine::ObjectPtr> selected_objects;

	// bound sphere properties
	Unigine::Math::Vec3 selected_objects_bound_sphere_position;
	float selected_objects_bound_sphere_radius;
};