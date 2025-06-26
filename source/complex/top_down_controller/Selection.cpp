#include "Selection.h"
#include "UnitSelectionCircle.h"
#include <UnigineVisualizer.h>
#include <UnigineWindowManager.h>
#include <UnigineGame.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(Selection);

using namespace Unigine;
using namespace Math;

Unigine::Math::Vec3 Selection::getCenter()
{
	updateBoundShpere();
	return selected_objects_bound_sphere_position;
}

float Selection::getBoundRadius()
{
	updateBoundShpere();
	return selected_objects_bound_sphere_radius;
}

bool Selection::hasSelection()
{
	return selected_objects.size() != 0;
}

void Selection::update()
{
	// works only when the Concole is inactive
	if (!Console::isActive())
	{
		Visualizer::setEnabled(true);

		// if the left mouse button is pressed, save cursor coordinates and set a flag indicating that selection has started
		if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
		{
			is_selection = true;
			selection_start_mouse_position = Input::getMousePosition() - WindowManager::getMainWindow()->getClientPosition();
		}

		// render selection box
		if (is_selection)
		{
			auto windowSize = WindowManager::getMainWindow()->getClientRenderSize();
			upper_left_selection_corner = vec2(selection_start_mouse_position.x * 1.0f / windowSize.x, 1.0f - selection_start_mouse_position.y * 1.0f / windowSize.y);
			ivec2 currentMousePosition = Input::getMousePosition() - WindowManager::getMainWindow()->getClientPosition();
			bottom_right_selection_corner = vec2(currentMousePosition.x * 1.0f / windowSize.x, 1.0f - currentMousePosition.y * 1.0f / windowSize.y);

			Visualizer::renderRectangle(vec4(upper_left_selection_corner, bottom_right_selection_corner), vec4_green);
		}

		// when the left mouse button is released, start looking for objects in selection box
		if (Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT))
		{
			PlayerPtr camera = Game::getPlayer();

			// unselect previously selected objects
			for (const ObjectPtr& it : selected_objects)
			{
				auto unitSelectionComponent = getComponent<UnitSelectionCircle>(it);
				unitSelectionComponent->setSelected(false);
			}

			// setting a flag indicating that selection action has stopped and calculate width and height of selection box
			is_selection = false;

			// getting perspective projection matrix for frustum
			ivec2 selectionFinishPosition(Input::getMousePosition() - WindowManager::getMainWindow()->getPosition());
			mat4 perspective = camera->getProjectionFromMainWindow(selection_start_mouse_position.x, selection_start_mouse_position.y, selectionFinishPosition.x, selectionFinishPosition.y);

			// setting frustum matrices
			frustum.set(perspective, inverse(camera->getWorldTransform()));
			// getting objects inside the selection box with initial mouse coordinates
			World::getIntersection(frustum, selected_objects);
			if (selected_objects.size() == 0)
			{
				vec3 dir(camera->getDirectionFromMainWindow(Input::getMousePosition().x, Input::getMousePosition().y));
				ObjectPtr objectUnderCursor = World::getIntersection(camera->getWorldPosition(), camera->getWorldPosition() + Vec3(dir) * 1000, ~0);
				if (objectUnderCursor)
					selected_objects.push_back(objectUnderCursor);
			}

			// mark selected objects
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
