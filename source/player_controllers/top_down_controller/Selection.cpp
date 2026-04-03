// Handles RTS-style box selection: drag to create selection rectangle,
// release to select all units within frustum. Single-click selects the
// object under cursor. Selected objects display selection circles.

#include "Selection.h"
#include "UnitSelectionCircle.h"
#include <UnigineVisualizer.h>
#include <UnigineWindowManager.h>
#include <UnigineGame.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(Selection);

using namespace Unigine;
using namespace Math;

// Returns center of the combined bounding sphere of all selected objects.
Unigine::Math::Vec3 Selection::getCenter()
{
	updateBoundShpere();
	return selected_objects_bound_sphere_position;
}

// Returns radius of the combined bounding sphere (scaled 4x for camera framing).
float Selection::getBoundRadius()
{
	updateBoundShpere();
	return selected_objects_bound_sphere_radius;
}

// Returns true if any objects are currently selected.
bool Selection::hasSelection()
{
	return selected_objects.size() != 0;
}

// Handles selection input: LMB down starts drag, LMB up ends and queries objects.
// Renders selection rectangle while dragging.
void Selection::update()
{
	// Works only when the Console is inactive
	if (!Console::isActive())
	{
		Visualizer::setEnabled(true);

		// If the left mouse button is pressed, save cursor coordinates and start selection
		if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
		{
			is_selection = true;
			selection_start_mouse_position = Input::getMousePosition() - WindowManager::getMainWindow()->getClientPosition();
		}

		// Render selection box
		if (is_selection)
		{
			auto windowSize = WindowManager::getMainWindow()->getClientRenderSize();
			upper_left_selection_corner = vec2(selection_start_mouse_position.x * 1.0f / windowSize.x, 1.0f - selection_start_mouse_position.y * 1.0f / windowSize.y);
			ivec2 currentMousePosition = Input::getMousePosition() - WindowManager::getMainWindow()->getClientPosition();
			bottom_right_selection_corner = vec2(currentMousePosition.x * 1.0f / windowSize.x, 1.0f - currentMousePosition.y * 1.0f / windowSize.y);

			Visualizer::renderRectangle(vec4(upper_left_selection_corner, bottom_right_selection_corner), vec4_green);
		}

		// When the left mouse button is released, query objects in selection box
		if (Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT))
		{
			PlayerPtr camera = Game::getPlayer();

			// Unselect previously selected objects
			for (const ObjectPtr& it : selected_objects)
			{
				auto unitSelectionComponent = getComponent<UnitSelectionCircle>(it);
				unitSelectionComponent->setSelected(false);
			}

			// Selection ended - build frustum from screen rectangle
			is_selection = false;

			// Build a frustum from the selection rectangle for spatial query.
		// The frustum extends from camera through the screen-space box.
			ivec2 selectionFinishPosition(Input::getMousePosition() - WindowManager::getMainWindow()->getPosition());
			mat4 perspective = camera->getProjectionFromMainWindow(selection_start_mouse_position.x, selection_start_mouse_position.y, selectionFinishPosition.x, selectionFinishPosition.y);

			frustum.set(perspective, inverse(camera->getWorldTransform()));
			// Find all objects whose bounds intersect the selection frustum
			World::getIntersection(frustum, selected_objects);
			if (selected_objects.size() == 0)
			{
				vec3 dir(camera->getDirectionFromMainWindow(Input::getMousePosition().x, Input::getMousePosition().y));
				ObjectPtr objectUnderCursor = World::getIntersection(camera->getWorldPosition(), camera->getWorldPosition() + Vec3(dir) * 1000, ~0);
				if (objectUnderCursor)
					selected_objects.push_back(objectUnderCursor);
			}

			// Mark selected objects and filter out non-selectable ones
			for (int i = 0; i < selected_objects.size(); i++)
			{
				auto unitSelectionComponent = getComponent<UnitSelectionCircle>(selected_objects[i]);
				if (unitSelectionComponent)
				{
					unitSelectionComponent->setSelected(true);
				}
				else
				{
					selected_objects.removeFast(i);
					i--;
				}
			}
			updateBoundShpere();
		}
	}
}

// Computes combined bounding sphere of all selected objects.
void Selection::updateBoundShpere()
{
	WorldBoundSphere bs;
	for (int i = 0; i < selected_objects.size(); i++)
	{
		bs.expand(selected_objects[i]->getWorldBoundSphere());
	}
	selected_objects_bound_sphere_position = bs.center;
	selected_objects_bound_sphere_radius = (float)bs.radius * 4.0f;
}
