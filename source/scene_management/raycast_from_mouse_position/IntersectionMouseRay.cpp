// Casts a ray from the camera through the mouse cursor into the 3D scene. Detects
// intersections with objects whose masks match and displays the hit object's name.
// Uses getDirectionFromMainWindow() to convert screen coordinates to world direction.

#include "IntersectionMouseRay.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(IntersectionMouseRay)

using namespace Unigine;
using namespace Math;

// Mouse handle is configured and hit label widget is created.
void IntersectionMouseRay::init()
{
	// Cursor is shown when player is not rotating
	init_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);

	// Label for target object name is created
	current_hit_label = WidgetLabel::create(Gui::getCurrent());
	current_hit_label->setFontSize(30);
	current_hit_label->setFontOutline(1);
	Gui::getCurrent()->addChild(current_hit_label, Gui::ALIGN_OVERLAP);
}

// Ray is cast from camera through mouse position; hit object name is displayed.
void IntersectionMouseRay::update()
{
	PlayerPtr current_player = Game::getPlayer();

	// 2D screen coordinates are converted to a 3D world-space direction vector.
	// Camera projection and viewport mapping are handled by getDirectionFromMainWindow().
	ivec2 mouse_coord = Input::getMousePosition();
	Vec3 direction = Vec3(current_player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y));

	// Ray segment is defined: start at camera position, extend along direction
	Vec3 first_point = current_player->getWorldPosition();
	Vec3 second_point = first_point + (direction * distance);

	// Ray is cast and intersection with scene objects is checked.
	// Only objects whose intersection mask overlaps with 'mask' are tested.
	Unigine::ObjectPtr hit_object = World::getIntersection(first_point, second_point, mask);
	if (hit_object)
	{
		current_hit_label->setText(hit_object->getName());
	}
	else
	{
		current_hit_label->setText("empty hit object");
	}

	// Label is positioned near the cursor with a small offset to avoid overlap
	GuiPtr window_gui = WindowManager::getMainWindow()->getGui();
	current_hit_label->setPosition(window_gui->getMouseX() + 25, window_gui->getMouseY() + 25);
}

// Mouse handle is restored and label widget is removed.
void IntersectionMouseRay::shutdown()
{
	Input::setMouseHandle(init_handle);
	Gui::getCurrent()->removeChild(current_hit_label);
}
