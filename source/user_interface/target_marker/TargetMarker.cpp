#include "TargetMarker.h"

REGISTER_COMPONENT(TargetMarker);

using namespace Unigine;
using namespace Math;

void TargetMarker::init()
{
	if (!arrowSprite)
	{
		Log::error("TargetMarker::init(): Source file for the pointer sprite image is not found.");
		return;
	}
	arrow = WidgetSprite::create(arrowSprite.get());
	WindowManager::getMainWindow()->addChild(arrow, Gui::ALIGN_OVERLAP);

	if (!pointSprite)
	{
		Log::error("TargetMarker::init(): Source file for the marker sprite image is not found.");
		return;
	}

	point = WidgetSprite::create(pointSprite.get());
	WindowManager::getMainWindow()->addChild(point, Gui::ALIGN_OVERLAP);

	if (!targetNode)
	{
		Log::error("TargetMarker::init(): No target object specified.");
		return;
	}
	target = targetNode.get();

	camera = checked_ptr_cast<Player>(node);
	if (!camera)
	{
		Log::error("TargetMarker::init(): Camera is not valid.");
		return;
	}
}

void TargetMarker::update()
{
	if (!arrow || !point || !target || !camera)
		return;

	arrow_height = arrow->getLayerHeight(0);
	arrow_width = arrow->getLayerWidth(0);
	arrow_half_height = arrow_height / 2;
	arrow_half_width = arrow_width / 2;

	point_width = point->getLayerWidth(0);
	point_height = point->getLayerHeight(0);
	mat4 translation = mat4(translate(Vec3(-point_width * pointPivot.get().x, -point_height * pointPivot.get().y, 0.0f) * WindowManager::getMainWindow()->getDpiScale()));
	point->setTransform(translation);

	arrow->setHidden(true);
	point->setHidden(true);

	// window size
	int width = WindowManager::getMainWindow()->getClientSize().x;
	int height = WindowManager::getMainWindow()->getClientSize().y;
	int halfWidth = width / 2;
	int halfHeight = height / 2;

	// target point in screen space
	int x = 0;
	int y = 0;

	vec3 targetDirection(target->getWorldBoundBox().getCenter() - camera->getWorldPosition());

	bool behind = dot(camera->getWorldDirection(Unigine::Math::AXIS_NZ), targetDirection) < 0;

	// find target point projection to the screen-space plane and change the coordinate system to the center of the screen
	if (!behind)
	{
		camera->getScreenPosition(x, y, target->getWorldBoundBox().getCenter(), width, height);

		x -= halfWidth;
		y -= halfHeight;
		y *= -1;
	}
	// if the target is out of sight calculate the position in screen space relative to the reflected target position
	else
	{
		Vec3 inverseScreenPlaneNormal(camera->getViewDirection() * -1);
		Vec3 relativeToCameraTargetPosition = target->getWorldBoundBox().getCenter() - camera->getWorldPosition();

		// ortho projection of vector realtiveToCameraTargetPosition to vector inverseScreenPlaneNormal
		Vec3 orthoProjectionTarget = inverseScreenPlaneNormal * dot(relativeToCameraTargetPosition, inverseScreenPlaneNormal);
		Vec3 reflectedTargetPosition = (relativeToCameraTargetPosition - orthoProjectionTarget * 2) + camera->getWorldPosition();

		camera->getScreenPosition(x, y, reflectedTargetPosition, width, height);

		x -= halfWidth;
		y -= halfHeight;
		// when target is out of sight the arrow is always at the bottom part of the screen
		if (y > 0)
			y *= -1;
	}

	// if the target is inside the field of view we highlight it with the marker
	if (!behind && x >= -halfWidth && x <= halfWidth && y >= -halfHeight && y <= halfHeight)
	{
		point->setHidden(false);
		point->setPosition(x + halfWidth, -y + halfHeight);
	}
	// if the target is out of sight we show an arrow that points direction to the target
	else
	{
		// calculating the pivot point for the arrow
		int point_x = 0;
		int point_y = 0;
		getIntersectionWithRect(point_x, point_y, x, y, halfWidth, halfHeight);
		float angle = 0.0f;

		// setting porsition and rotation of the arrow sprite, all calculations are made taking into account the fact that the position of the sprite is calculated relative to its upper left corner
		// if the arrow is in the corner
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
		// if the arrow points up
		else if (point_y == halfHeight)
		{
			point_x -= arrow_half_width;
			angle = -90;
		}
		// if the arrow points down
		else if (point_y == -halfHeight)
		{
			point_y += arrow_height;
			point_x -= arrow_half_width;
			angle = 90;
		}
		// if the arrow points to the left
		else if (point_x == -halfWidth)
		{
			point_y += arrow_half_height;
			angle = 180;
		}
		// if the arrow points to the right
		else if (point_x == halfWidth)
		{
			point_x -= arrow_width;
			point_y += arrow_half_height;
			angle = 0;
		}

		arrow->setHidden(false);
		arrow->setPosition(point_x + halfWidth, -point_y + halfHeight);

		mat4 rotation(
			mat4(translate(Vec3(arrow_half_width * 1.0f, arrow_half_height * 1.0f, 0.0f) * WindowManager::getMainWindow()->getDpiScale())) *
			rotate(quat(vec3_up, angle)) *
			mat4(translate(Vec3(-arrow_half_width * 1.0f, -arrow_half_height * 1.0f, 0.0f) * WindowManager::getMainWindow()->getDpiScale()))
		);

		arrow->setLayerTransform(0, rotation);
	}
}

void TargetMarker::shutdown()
{
	arrow.deleteLater();
	point.deleteLater();
}

void TargetMarker::getIntersectionWithRect(int& x, int& y, int vec_x, int vec_y, int half_width, int half_height)
{
	if (vec_y >= 0)
	{
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

		x = (int)(half_height * ((float)vec_x / (float)vec_y));
		y = half_height;

		if (x >= -half_width && x <= half_width)
			return;

		if (vec_x >= 0)
		{
			if (vec_x == 0)
			{
				x = 0;
				y = half_height;

				return;
			}

			x = half_width;
			y = (int)(half_width * ((float)vec_y / (float)vec_x));

			return;
		}
		else
		{
			x = -half_width;
			y = (int)(-half_width * ((float)vec_y / (float)vec_x));

			return;
		}
	}
	else
	{
		x = (int)(-half_height * ((float)vec_x / (float)vec_y));
		y = -half_height;

		if (x >= -half_width && x <= half_width)
			return;

		if (vec_x >= 0)
		{
			if (vec_x == 0)
			{
				x = 0;
				y = -half_height;

				return;
			}

			x = half_width;
			y = (int)(half_width * ((float)vec_y / (float)vec_x));

			return;
		}
		else
		{
			x = -half_width;
			y = (int)(-half_width * ((float)vec_y / (float)vec_x));

			return;
		}
	}
}
