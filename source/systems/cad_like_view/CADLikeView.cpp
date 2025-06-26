#include "CADLikeView.h"

#include "ViewportCube.h"

#include <UnigineConsole.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWindowManager.h>

REGISTER_COMPONENT(CADLikeView);

using namespace Unigine;
using namespace Math;

void CADLikeView::init()
{
	Render::setEnabled(false);

	camera_top = checked_ptr_cast<Player>(player_top.get())->getCamera();
	camera_perspective = checked_ptr_cast<Player>(player_perspective.get())->getCamera();
	camera_side = checked_ptr_cast<Player>(player_side.get())->getCamera();
	camera_front = checked_ptr_cast<Player>(player_front.get())->getCamera();

	set_ortho_projection(ortho_width.get(), ortho_height.get());
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);

	create_viewports();
	create_viewcubes();
	update_viewcubes_position();
}

void CADLikeView::create_viewports()
{
	const ivec2 window_size = WindowManager::getMainWindow()->getSize();

	current_width = window_size.x;
	current_height = window_size.y;

	const int viewport_width = current_width / 2;
	const int viewport_height = current_height / 2;

	const auto gui = Gui::getCurrent();

	viewport_top = WidgetSpriteViewport::create(gui, viewport_width, viewport_height);
	viewport_perspective = WidgetSpriteViewport::create(gui, viewport_width, viewport_height);
	viewport_side = WidgetSpriteViewport::create(gui, viewport_width, viewport_height);
	viewport_front = WidgetSpriteViewport::create(gui, viewport_width, viewport_height);

	lay = WidgetGridBox::create(gui, 2);
	gui->addChild(lay, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND | Gui::ALIGN_EXPAND);

	lay->addChild(viewport_top);
	lay->addChild(viewport_perspective);
	lay->addChild(viewport_side);
	lay->addChild(viewport_front);
}

void CADLikeView::set_ortho_projection(const float width, const float height)
{
	constexpr float z_near = 0.001f;
	constexpr float z_far = 1000.f;
	const mat4 ortho_projection = ortho(-width / 2, width / 2, -height / 2, height / 2, z_near,
		z_far);
	camera_top->setProjection(ortho_projection);
	camera_side->setProjection(ortho_projection);
	camera_front->setProjection(ortho_projection);
}

void CADLikeView::create_viewcubes()
{
	viewcube_top = std::make_unique<ViewportCube>(checked_ptr_cast<Player>(player_top.get()),
		ortho_width, ortho_height);
	viewcube_perspective = std::make_unique<ViewportCube>(
		checked_ptr_cast<Player>(player_perspective.get()), ortho_width, ortho_height);
	viewcube_side = std::make_unique<ViewportCube>(checked_ptr_cast<Player>(player_side.get()),
		ortho_width, ortho_height);
	viewcube_front = std::make_unique<ViewportCube>(checked_ptr_cast<Player>(player_front.get()),
		ortho_width, ortho_height);

	viewcube_top->setChangeProjectionEnabled(false);
	viewcube_side->setChangeProjectionEnabled(false);
	viewcube_front->setChangeProjectionEnabled(false);

	const Vec3 centering_position = figure.get()->getHierarchyWorldBoundSphere().center;
	viewcube_top->setCenteringPosition(centering_position);
	viewcube_perspective->setCenteringPosition(centering_position);
	viewcube_side->setCenteringPosition(centering_position);
	viewcube_front->setCenteringPosition(centering_position);

	const Scalar min_distance_multiplier = 2.0f;
	const Scalar min_distance = figure.get()->getHierarchyWorldBoundSphere().radius
		* min_distance_multiplier;
	viewcube_top->setMinimalDistance(min_distance);
	viewcube_perspective->setMinimalDistance(min_distance);
	viewcube_side->setMinimalDistance(min_distance);
	viewcube_front->setMinimalDistance(min_distance);
}

void CADLikeView::update_viewcubes_position()
{
	const ivec2 cube_offset(-130, 10);

	viewcube_top->setViewcubePosition(ivec2(current_width / 2, 0) + cube_offset);
	viewcube_perspective->setViewcubePosition(ivec2(current_width, 0) + cube_offset);
	viewcube_side->setViewcubePosition(ivec2(current_width / 2, current_height / 2) + cube_offset);
	viewcube_front->setViewcubePosition(ivec2(current_width, current_height / 2) + cube_offset);
}

void CADLikeView::update()
{
	if (!WindowManager::getMainWindow())
		return;

	update_screen_size();
	update_input();

	viewport_top->setCamera(camera_top);
	viewport_perspective->setCamera(camera_perspective);
	viewport_side->setCamera(camera_side);
	viewport_front->setCamera(camera_front);

	viewcube_top->update();
	viewcube_perspective->update();
	viewcube_side->update();
	viewcube_front->update();
}

void CADLikeView::update_screen_size()
{
	const ivec2 window_size = WindowManager::getMainWindow()->getSize();

	bool need_update_viewcubes = false;

	const int window_width = window_size.x;
	if (window_width != current_width)
	{
		need_update_viewcubes = true;
		current_width = window_width;
		const int viewport_width = current_width / 2;
		viewport_top->setTextureWidth(viewport_width);
		viewport_perspective->setTextureWidth(viewport_width);
		viewport_side->setTextureWidth(viewport_width);
		viewport_front->setTextureWidth(viewport_width);
	}

	const int window_height = window_size.y;
	if (window_height != current_height)
	{
		need_update_viewcubes = true;
		current_height = window_height;
		const int viewport_height = window_height / 2;
		viewport_top->setTextureHeight(viewport_height);
		viewport_perspective->setTextureHeight(viewport_height);
		viewport_side->setTextureHeight(viewport_height);
		viewport_front->setTextureHeight(viewport_height);
	}

	if (need_update_viewcubes)
	{
		update_viewcubes_position();
	}
}

void CADLikeView::update_input()
{
	if (Console::isActive())
		return;

	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		set_dragged_player();
	}

	if (Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT))
	{
		checked_ptr_cast<Player>(player_perspective.get())->setControlled(false);
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
		dragged_player = PlayerPtr();
	}

	if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON_LEFT))
	{
		if (dragged_player)
		{
			const int viewport_width = current_width / 2;
			const int viewport_height = current_height / 2;

			const Vec2 normalized_delta = Vec2(Input::getMouseDeltaPosition())
				/ Vec2((float)viewport_width, (float)viewport_height);
			Vec2 delta_pos = normalized_delta
				* Vec2((itof(current_width) / current_height) * ortho_width, ortho_height);
			delta_pos.x *= -1;
			dragged_player->translate({delta_pos, 0});
		}
	}
}

void CADLikeView::set_dragged_player()
{
	if (viewcube_top->isHovered() || viewcube_perspective->isHovered() || viewcube_side->isHovered()
		|| viewcube_front->isHovered())
	{
		return;
	}

	const int viewport_width = current_width / 2;
	const int viewport_height = current_height / 2;

	const ivec2 mouse_pos = Input::getMousePosition()
		- WindowManager::getMainWindow()->getClientPosition();

	if (checkRange(mouse_pos.x, 0, viewport_width))
	{
		if (checkRange(mouse_pos.y, 0, viewport_height))
		{
			dragged_player = checked_ptr_cast<Player>(player_top.get());
		}
		else if (checkRange(mouse_pos.y, viewport_height, current_height))
		{
			dragged_player = checked_ptr_cast<Player>(player_side.get());
		}
	}
	else if (checkRange(mouse_pos.x, viewport_width, current_width))
	{
		if (checkRange(mouse_pos.y, 0, viewport_height))
		{
			Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);
			checked_ptr_cast<Player>(player_perspective.get())->setControlled(true);
		}
		else if (checkRange(mouse_pos.y, viewport_height, current_height))
		{
			dragged_player = checked_ptr_cast<Player>(player_front.get());
		}
	}
}

void CADLikeView::shutdown()
{
	Render::setEnabled(true);
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	viewcube_top.reset();
	viewcube_perspective.reset();
	viewcube_side.reset();
	viewcube_front.reset();

	lay.deleteLater();
	viewport_top.deleteLater();
	viewport_perspective.deleteLater();
	viewport_side.deleteLater();
	viewport_front.deleteLater();
}
