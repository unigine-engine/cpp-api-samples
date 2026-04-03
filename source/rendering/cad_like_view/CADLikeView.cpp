// CAD-style 2x2 viewport layout: top/side/front orthographic views plus
// one perspective view. Each view has an orientation cube for camera control.
// Supports drag-to-pan in orthographic views and free camera in perspective.

#include "CADLikeView.h"

#include "ViewportCube.h"

#include <UnigineConsole.h>
#include <UniginePlayers.h>
#include <UnigineViewport.h>
#include <UnigineWindowManager.h>

REGISTER_COMPONENT(CADLikeView);

using namespace Unigine;
using namespace Math;

// Initializes cameras, creates viewport widgets and viewcubes,
// and sets up orthographic projection for technical views.
void CADLikeView::init()
{
	// Disable engine rendering while setting up viewports and cameras
	Render::setEnabled(false);

	// Retrieve cameras from players
	camera_top = checked_ptr_cast<Player>(player_top.get())->getCamera();
	camera_perspective = checked_ptr_cast<Player>(player_perspective.get())->getCamera();
	camera_side = checked_ptr_cast<Player>(player_side.get())->getCamera();
	camera_front = checked_ptr_cast<Player>(player_front.get())->getCamera();

	// Set orthographic projection for top/side/front views
	set_ortho_projection(ortho_width.get(), ortho_height.get());
	last_handle = Input::getMouseHandle();
	Input::setMouseHandle(Input::MOUSE_HANDLE_USER);

	// Create viewport widgets and orientation viewcubes
	create_viewports();
	create_viewcubes();
	update_viewcubes_position();
}

// Creates four WidgetSpriteViewport widgets arranged in a 2x2 GridBox.
void CADLikeView::create_viewports()
{
	// Divide main window into 2x2 grid and create viewports for each view
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

// Applies orthographic projection matrix to top, side, and front cameras.
void CADLikeView::set_ortho_projection(const float width, const float height)
{
	// Apply orthographic projection to top/side/front cameras
	constexpr float z_near = 0.001f;
	constexpr float z_far = 1000.f;
	const mat4 ortho_projection = ortho(-width / 2, width / 2, -height / 2, height / 2, z_near,
		z_far);
	camera_top->setProjection(ortho_projection);
	camera_side->setProjection(ortho_projection);
	camera_front->setProjection(ortho_projection);
}

// Creates ViewportCube instances for each viewport, sets centering position
// and minimal distance based on the target figure's bounding sphere.
void CADLikeView::create_viewcubes()
{
	// Create orientation cubes for each viewport
	viewcube_top = std::make_unique<ViewportCube>(checked_ptr_cast<Player>(player_top.get()),
		ortho_width, ortho_height);
	viewcube_perspective = std::make_unique<ViewportCube>(
		checked_ptr_cast<Player>(player_perspective.get()), ortho_width, ortho_height);
	viewcube_side = std::make_unique<ViewportCube>(checked_ptr_cast<Player>(player_side.get()),
		ortho_width, ortho_height);
	viewcube_front = std::make_unique<ViewportCube>(checked_ptr_cast<Player>(player_front.get()),
		ortho_width, ortho_height);

	// Lock viewcube projection changes for orthographic views
	viewcube_top->setChangeProjectionEnabled(false);
	viewcube_side->setChangeProjectionEnabled(false);
	viewcube_front->setChangeProjectionEnabled(false);

	// Center viewcubes on the target figure
	const Vec3 centering_position = figure.get()->getHierarchyWorldBoundSphere().center;
	viewcube_top->setCenteringPosition(centering_position);
	viewcube_perspective->setCenteringPosition(centering_position);
	viewcube_side->setCenteringPosition(centering_position);
	viewcube_front->setCenteringPosition(centering_position);

	// Set minimal distance to figure for proper visibility
	const Scalar min_distance_multiplier = 2.0f;
	const Scalar min_distance = figure.get()->getHierarchyWorldBoundSphere().radius
		* min_distance_multiplier;
	viewcube_top->setMinimalDistance(min_distance);
	viewcube_perspective->setMinimalDistance(min_distance);
	viewcube_side->setMinimalDistance(min_distance);
	viewcube_front->setMinimalDistance(min_distance);
}

// Positions viewcube widgets in the top-right corner of each viewport.
void CADLikeView::update_viewcubes_position()
{
	// Position viewcubes in corner of each viewport
	const ivec2 cube_offset(-130, 10);
	viewcube_top->setViewcubePosition(ivec2(current_width / 2, 0) + cube_offset);
	viewcube_perspective->setViewcubePosition(ivec2(current_width, 0) + cube_offset);
	viewcube_side->setViewcubePosition(ivec2(current_width / 2, current_height / 2) + cube_offset);
	viewcube_front->setViewcubePosition(ivec2(current_width, current_height / 2) + cube_offset);
}

// Per-frame update: handles window resize, input processing,
// camera assignment to viewports, and viewcube updates.
void CADLikeView::update()
{
	if (!WindowManager::getMainWindow())
		return;

	// Update viewport sizes if window resized
	update_screen_size();
	update_input();

	// Assign cameras to viewports
	viewport_top->setCamera(camera_top);
	viewport_perspective->setCamera(camera_perspective);
	viewport_side->setCamera(camera_side);
	viewport_front->setCamera(camera_front);

	// Update orientation cubes
	viewcube_top->update();
	viewcube_perspective->update();
	viewcube_side->update();
	viewcube_front->update();
}

// Detects window resize and updates viewport texture dimensions accordingly.
void CADLikeView::update_screen_size()
{
	// Adjust viewport textures and reposition viewcubes if window size changed
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

// Handles mouse input for viewport dragging and perspective camera control.
void CADLikeView::update_input()
{
	// Ignore input when the console is active
	if (Console::isActive())
		return;

	// Start dragging a player (camera) when the left mouse button is pressed
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		set_dragged_player();
	}

	// Stop dragging when the left mouse button is released
	if (Input::isMouseButtonUp(Input::MOUSE_BUTTON_LEFT))
	{
		checked_ptr_cast<Player>(player_perspective.get())->setControlled(false);
		Input::setMouseHandle(Input::MOUSE_HANDLE_USER);
		dragged_player = PlayerPtr();
	}

	// Continue dragging while holding the left mouse button
	if (Input::isMouseButtonPressed(Input::MOUSE_BUTTON_LEFT))
	{
		if (dragged_player)
		{
			const int viewport_width = current_width / 2;
			const int viewport_height = current_height / 2;

			// Normalize mouse delta relative to current viewport
			const Vec2 normalized_delta = Vec2(Input::getMouseDeltaPosition())
				/ Vec2((float)viewport_width, (float)viewport_height);

			// Scale delta into world coordinates according to orthographic projection
			Vec2 delta_pos = normalized_delta
				* Vec2((itof(current_width) / current_height) * ortho_width, ortho_height);
			delta_pos.x *= -1;

			// Move the player (camera) in the scene
			dragged_player->translate({delta_pos, 0});
		}
	}
}

// Determines which viewport the mouse is over and sets up dragging.
// Perspective viewport enables free camera control; others use drag-to-pan.
void CADLikeView::set_dragged_player()
{
	// Do not select a player if the mouse is over any viewcube
	if (viewcube_top->isHovered() || viewcube_perspective->isHovered() || viewcube_side->isHovered()
		|| viewcube_front->isHovered())
	{
		return;
	}

	const int viewport_width = current_width / 2;
	const int viewport_height = current_height / 2;

	// Get mouse position relative to the client area of the window
	const ivec2 mouse_pos = Input::getMousePosition()
		- WindowManager::getMainWindow()->getClientPosition();

	// Determine which player/camera the user intends to drag 
	// based on mouse position in the 2x2 viewport grid
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
			// For the perspective view, enable mouse control
			Input::setMouseHandle(Input::MOUSE_HANDLE_SOFT);
			checked_ptr_cast<Player>(player_perspective.get())->setControlled(true);
		}
		else if (checkRange(mouse_pos.y, viewport_height, current_height))
		{
			dragged_player = checked_ptr_cast<Player>(player_front.get());
		}
	}
}

// Cleans up viewports, viewcubes, and restores rendering/input settings.
void CADLikeView::shutdown()
{
	// Restore engine rendering and input handling, cleanup viewcubes and layout
	Render::setEnabled(true);
	Input::setMouseHandle(last_handle);

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
