// RTS-style box selection handler. Supports drag-to-select with visual rectangle,
// frustum-based spatial query for objects, and single-click selection fallback.

#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineGame.h>

// Handles mouse-based unit selection: drag rectangle selects all units within
// the frustum, single click selects object under cursor. Tracks selected objects
// and computes their combined bounding sphere for camera focus.
class Selection : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Selection, Unigine::ComponentBase);

	COMPONENT_UPDATE(update);

	Unigine::Math::Vec3 getCenter();	// Returns center of selected objects' bounding sphere
	float getBoundRadius();				// Returns radius of selected objects' bounding sphere

	bool hasSelection();	// Returns true if any objects are selected

protected:
	void update();

private:
	void updateBoundShpere();	// Note: typo in method name - should be "updateBoundSphere"

	bool is_selection = false;	// True while drag-selecting

	// Selection rectangle in screen coordinates
	Unigine::Math::ivec2 selection_start_mouse_position;	// Window-space position at drag start
	Unigine::Math::vec2 upper_left_selection_corner;		// Normalized [0,1] position of rectangle corner
	Unigine::Math::vec2 bottom_right_selection_corner;		// Normalized [0,1] position of rectangle corner

	Unigine::Math::WorldBoundFrustum frustum;			// Frustum built from selection rectangle
	Unigine::Vector<Unigine::ObjectPtr> selected_objects;	// Currently selected objects

	// Combined bounding sphere of all selected objects
	Unigine::Math::Vec3 selected_objects_bound_sphere_position;
	float selected_objects_bound_sphere_radius;
};