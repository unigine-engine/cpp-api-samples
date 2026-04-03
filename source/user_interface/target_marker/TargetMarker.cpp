// Implements an off-screen target indicator system. When the target is visible,
// a marker sprite highlights its position. When off-screen, an arrow sprite
// rotates to point toward the target direction with edge-clamped positioning.

#include "TargetMarker.h"

REGISTER_COMPONENT(TargetMarker);

using namespace Unigine;
using namespace Math;

// Sprites are loaded and attached to main window, target and camera are validated.
void TargetMarker::init()
{
	// Load arrow sprite for off-screen indication
	if (!arrowSprite)
	{
		Log::error("TargetMarker::init(): Source file for the pointer sprite image is not found.");
		return;
	}
	arrow = WidgetSprite::create(arrowSprite.get());
	WindowManager::getMainWindow()->addChild(arrow, Gui::ALIGN_OVERLAP);

	// Load point marker sprite for on-screen highlighting
	if (!pointSprite)
	{
		Log::error("TargetMarker::init(): Source file for the marker sprite image is not found.");
		return;
	}

	point = WidgetSprite::create(pointSprite.get());
	WindowManager::getMainWindow()->addChild(point, Gui::ALIGN_OVERLAP);

	// Validate target node reference
	if (!targetNode)
	{
		Log::error("TargetMarker::init(): No target object specified.");
		return;
	}
	target = targetNode.get();

	// Component must be attached to a Player node for projection calculations
	camera = checked_ptr_cast<Player>(node);
	if (!camera)
	{
		Log::error("TargetMarker::init(): Camera is not valid.");
		return;
	}
}

// Target position is projected to screen and appropriate indicator is displayed.
void TargetMarker::update()
{
	if (!arrow || !point || !target || !camera)
		return;

	// Cache sprite dimensions for arrow positioning calculations
	arrow_height = arrow->getLayerHeight(0);
	arrow_width = arrow->getLayerWidth(0);
	arrow_half_height = arrow_height / 2;
	arrow_half_width = arrow_width / 2;

	point_width = point->getLayerWidth(0);
	point_height = point->getLayerHeight(0);
	// Offset marker by configurable pivot, scaled for display DPI
	mat4 translation = mat4(translate(Vec3(-point_width * pointPivot.get().x, -point_height * pointPivot.get().y, 0.0f) * WindowManager::getMainWindow()->getDpiScale()));
	point->setTransform(translation);

	// Start with both hidden; visibility is determined below based on target position
	arrow->setHidden(true);
	point->setHidden(true);

	// Window size
	int width = WindowManager::getMainWindow()->getClientSize().x;
	int height = WindowManager::getMainWindow()->getClientSize().y;
	int halfWidth = width / 2;
	int halfHeight = height / 2;

	// Target point in screen space
	int x = 0;
	int y = 0;

	// Direction vector from camera to target center
	vec3 targetDirection(target->getWorldBoundBox().getCenter() - camera->getWorldPosition());

	// Negative dot product indicates target is behind the camera's view plane
	bool behind = dot(camera->getWorldDirection(Unigine::Math::AXIS_NZ), targetDirection) < 0;

	// Find target point projection to the screen-space plane and change the coordinate system to the center of the screen
	if (!behind)
	{
		camera->getScreenPosition(x, y, target->getWorldBoundBox().getCenter(), width, height);

		x -= halfWidth;
		y -= halfHeight;
		y *= -1;
	}
	// If target is out of sight calculate position in screen space relative to reflected target position
	else
	{
		Vec3 inverseScreenPlaneNormal(camera->getViewDirection() * -1);
		Vec3 relativeToCameraTargetPosition = target->getWorldBoundBox().getCenter() - camera->getWorldPosition();

		// Orthogonal projection of relativeToCameraTargetPosition onto inverseScreenPlaneNormal
		Vec3 orthoProjectionTarget = inverseScreenPlaneNormal * dot(relativeToCameraTargetPosition, inverseScreenPlaneNormal);
		Vec3 reflectedTargetPosition = (relativeToCameraTargetPosition - orthoProjectionTarget * 2) + camera->getWorldPosition();

		camera->getScreenPosition(x, y, reflectedTargetPosition, width, height);

		x -= halfWidth;
		y -= halfHeight;
		// When target is out of sight the arrow is always at the bottom part of the screen
		if (y > 0)
			y *= -1;
	}

	// If target is inside the field of view, highlight it with the marker
	if (!behind && x >= -halfWidth && x <= halfWidth && y >= -halfHeight && y <= halfHeight)
	{
		point->setHidden(false);
		point->setPosition(x + halfWidth, -y + halfHeight);
	}
	// If target is out of sight, show an arrow pointing toward it
	else
	{
		// Calculate the pivot point for the arrow
		int point_x = 0;
		int point_y = 0;
		getIntersectionWithRect(point_x, point_y, x, y, halfWidth, halfHeight);
		float angle = 0.0f;

		// Set position and rotation of arrow sprite (sprite position is relative to its upper left corner)
		// Arrow is in the corner
		if (halfHeight - Math::abs(point_y) <= arrow_half_height && halfWidth - Math::abs(point_x) <= arrow_half_width)
		{
			float dx, dy;

			if (point_y > 0)
				dy = (float)(point_y - (halfHeight - arrow_half_width));
			else
				dy = (float)(point_y + (halfHeight - arrow_half_width));

			if (point_x > 0)
				dx = (float)(point_x - (halfWidth - arrow_half_width));
			else
				dx = (float)(point_x + (halfWidth - arrow_half_width));

			angle = -Math::atan2(dy, dx) * Consts::RAD2DEG;

			if (point_x > 0)
				point_x = halfWidth - arrow_width;
			else
				point_x = -halfWidth;

			if (point_y > 0)
				point_y = halfHeight;
			else
				point_y = -halfHeight + arrow_height;
		}
		// Arrow points up
		else if (point_y == halfHeight)
		{
			point_x -= arrow_half_width;
			angle = -90;
		}
		// Arrow points down
		else if (point_y == -halfHeight)
		{
			point_y += arrow_height;
			point_x -= arrow_half_width;
			angle = 90;
		}
		// Arrow points to the left
		else if (point_x == -halfWidth)
		{
			point_y += arrow_half_height;
			angle = 180;
		}
		// Arrow points to the right
		else if (point_x == halfWidth)
		{
			point_x -= arrow_width;
			point_y += arrow_half_height;
			angle = 0;
		}

		arrow->setHidden(false);
		// Convert from center-origin coords back to top-left origin for widget positioning
		arrow->setPosition(point_x + halfWidth, -point_y + halfHeight);

		// Rotate arrow around its center: translate to origin, rotate, translate back
		mat4 rotation(
			mat4(translate(Vec3(arrow_half_width * 1.0f, arrow_half_height * 1.0f, 0.0f) * WindowManager::getMainWindow()->getDpiScale())) *
			rotate(quat(vec3_up, angle)) *
			mat4(translate(Vec3(-arrow_half_width * 1.0f, -arrow_half_height * 1.0f, 0.0f) * WindowManager::getMainWindow()->getDpiScale()))
		);

		arrow->setLayerTransform(0, rotation);
	}
}

// Sprite widgets are released.
void TargetMarker::shutdown()
{
	arrow.deleteLater();
	point.deleteLater();
}

// Calculates where a ray from screen center intersects the screen boundary rectangle.
// Used to clamp the arrow indicator position to screen edges when target is off-screen.
void TargetMarker::getIntersectionWithRect(int& x, int& y, int vec_x, int vec_y, int half_width, int half_height)
{
	// Upper half of screen (positive Y in center-origin coords)
	if (vec_y >= 0)
	{
		// horizontal ray - intersects left or right edge only
		if (vec_y == 0)
		{
			if (vec_x > 0)
			{
				x = half_width;
				y = 0;
			}
			else
			{
				x = -half_width;
				y = 0;
			}

			return;
		}

		// Try intersection with top edge first using similar triangles
		x = (int)(half_height * ((float)vec_x / (float)vec_y));
		y = half_height;

		// If intersection point is within screen width, top edge is the closest
		if (x >= -half_width && x <= half_width)
			return;

		// Otherwise intersect with left or right edge
		if (vec_x >= 0)
		{
			if (vec_x == 0)
			{
				x = 0;
				y = half_height;

				return;
			}

			// Intersection with right edge
			x = half_width;
			y = (int)(half_width * ((float)vec_y / (float)vec_x));

			return;
		}
		else
		{
			// Intersection with left edge
			x = -half_width;
			y = (int)(-half_width * ((float)vec_y / (float)vec_x));

			return;
		}
	}
	// Lower half of screen (negative Y in center-origin coords)
	else
	{
		// Try intersection with bottom edge first
		x = (int)(-half_height * ((float)vec_x / (float)vec_y));
		y = -half_height;

		// If within screen width, bottom edge is the closest intersection
		if (x >= -half_width && x <= half_width)
			return;

		// Otherwise intersect with left or right edge
		if (vec_x >= 0)
		{
			if (vec_x == 0)
			{
				x = 0;
				y = -half_height;

				return;
			}

			// Intersection with right edge
			x = half_width;
			y = (int)(half_width * ((float)vec_y / (float)vec_x));

			return;
		}
		else
		{
			// Intersection with left edge
			x = -half_width;
			y = (int)(-half_width * ((float)vec_y / (float)vec_x));

			return;
		}
	}
}
