// Interactive 3D orientation cube widget for camera navigation. Clicking on
// cube faces, edges, or corners animates camera rotation to that viewpoint.
// Supports ortho/perspective toggle and shows directional arrows when aligned.

#include "ViewportCube.h"

#include <UnigineGame.h>
#include <UniginePlayers.h>
#include "../../utils/Utils.h"

using namespace Unigine;
using namespace Math;

namespace
{
	// UI and cube display parameters
	const float VIEWCUBE_WIDTH = 120.0f;
	const float VIEWCUBE_HEIGHT = 120.0f;
	const float VIEWCUBE_PROJECTION_ICON_SIZE = 32.f;
	const float VIEWCUBE_SCALE = 55.0f;

	const float CAMERA_ANIMATION_PERIOD = 0.25f;
}

ViewportCube::ViewportCube(const PlayerPtr &player, float ortho_width, float ortho_height)
	: player(player), ortho_width(ortho_width), ortho_height(ortho_height)
{
	auto gui = Gui::getCurrent();

	// Ensure the player's up vector is aligned with Z-axis for correct cube orientation
	if (player->getType() != Node::NODE_DUMMY)
		player->setUp(normalize(vec3(0, 0.00000001, 1)));

	// Load cube resources (icons, meshes, textures) from ViewCubeSettings property
	auto prop = Properties::findProperty("ViewCubeSettings");
	if (!prop)
	{
		Log::error("The ViewCubeSettings component is not found!");
	}
	String viewcube_projection_icon_path = prop->getParameterPtr("VIEWCUBE_PROJECTION_ICON")->getValueString();
	String viewcube_mesh_path = prop->getParameterPtr("VIEWCUBE_MESH")->getValueString();
	String viewcube_arrows_mesh_path = prop->getParameterPtr("VIEWCUBE_ARROWS_MESH")->getValueString();
	viewcube_texture_touch_path = prop->getParameterPtr("VIEWCUBE_TEXTURE_TOUCH")->getValueString();
	viewcube_texture_untouch_path = prop->getParameterPtr("VIEWCUBE_TEXTURE_UNTOUCH")->getValueString();

	main_widget = WidgetHBox::create(gui);
	main_widget->setWidth((int)VIEWCUBE_WIDTH);
	main_widget->setHeight((int)VIEWCUBE_HEIGHT);

	// Projection toggle button (switch ortho/perspective)
	ortho_hbox = WidgetHBox::create(gui);
	main_widget->addChild(ortho_hbox, Gui::ALIGN_EXPAND);

	ortho_icon = WidgetIcon::create(gui, viewcube_projection_icon_path,
									 (int)VIEWCUBE_PROJECTION_ICON_SIZE,
									 (int)VIEWCUBE_PROJECTION_ICON_SIZE);
	ortho_icon->setToolTip("Switch between the orthographic and perspective projections");
	ortho_icon->setToggleable(true);
	ortho_icon->getEventChanged().connect(icon_connection, this, &ViewportCube::ortho_toggled);
	ortho_hbox->addChild(ortho_icon, Gui::ALIGN_RIGHT | Gui::ALIGN_TOP);

	// Build the cube canvas from the provided mesh. Each mesh surface represents a selectable
	// cube element: a face (eg. +x), an edge (combination like +x and +y), or a corner (+x and +y and +z).
	MeshPtr mesh = Mesh::create(viewcube_mesh_path);
	cube_canvas = WidgetCanvas::create(gui);

	// Create cube mesh canvas and store positions/rotations for each face
	for (int surface = 0; surface < mesh->getNumSurfaces(); ++surface)
	{
		const char *name = mesh->getSurfaceName(surface);
		vec3 pos = get_polygon_pos(name);
		quat rot = get_polygon_rot(pos);
		positions.push_back(pos);
		rotations.push_back(rot);

		mesh->remapCVertex(surface);
		int polygon = cube_canvas->addPolygon();
		cube_canvas->setPolygonTwoSided(polygon, 0);

		for (int i = 0; i < mesh->getNumVertex(surface); ++i)
		{
			vec3 point = mesh->getVertex(i, surface);
			cube_canvas->addPolygonPoint(polygon, point);

			vec2 uv = mesh->getTexCoord0(i, surface);
			cube_canvas->setPolygonTexCoord(polygon, uv);
		}

		for (int i = 0; i < mesh->getNumCIndices(surface); ++i)
		{
			int index = mesh->getCIndex(i, surface);
			cube_canvas->addPolygonIndex(polygon, index);
		}
	}

	mesh = Mesh::create(viewcube_arrows_mesh_path);
	arrows_canvas = WidgetCanvas::create(gui);

	// Create arrows mesh canvas for navigation indicators
	for (int surface = 0, num = mesh->getNumSurfaces(); surface < num; ++surface)
	{
		const char *name = mesh->getSurfaceName(surface);
		vec3 pos = get_polygon_pos(name);
		quat rot = get_polygon_rot(pos);
		rotations.push_back(rot);

		mesh->remapCVertex(surface);
		int polygon = arrows_canvas->addPolygon();

		for (int i = 0; i < mesh->getNumVertex(surface); ++i)
		{
			vec3 point = mesh->getVertex(i, surface);
			arrows_canvas->addPolygonPoint(polygon, point);

			vec2 uv = mesh->getTexCoord0(i, surface);
			arrows_canvas->setPolygonTexCoord(polygon, uv);
		}

		for (int i = 0; i < mesh->getNumCIndices(surface); ++i)
		{
			int index = mesh->getCIndex(i, surface);
			arrows_canvas->addPolygonIndex(polygon, index);
		}
	}

	// Setup transforms for cube in both orthographic and perspective modes
	ortho_transform = translate(VIEWCUBE_WIDTH / 2, VIEWCUBE_HEIGHT / 2, 0.0f)
		* scale(VIEWCUBE_SCALE, -VIEWCUBE_SCALE, VIEWCUBE_SCALE);

	perspective_transform = ortho_transform * perspective(40.0f, 1.0f, 0.0f, 100.0f)
		* translate(0, 0, -3.3f);

	for (int i = 0, num = cube_canvas->getNumPolygons(); i < num; ++i)
	{
		cube_canvas->setPolygonTexture(i, viewcube_texture_untouch_path);
	}

	for (int i = 0, num = arrows_canvas->getNumPolygons(); i < num; ++i)
	{
		arrows_canvas->setPolygonTexture(i, viewcube_texture_untouch_path);
	}

	arrows_canvas->setHidden(true);

	main_widget->addChild(arrows_canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);
	main_widget->addChild(cube_canvas, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	gui->addChild(main_widget, Gui::ALIGN_OVERLAP | Gui::ALIGN_FIXED);

	cube_canvas->setTransform(perspective_transform);
	arrows_canvas->setTransform(ortho_transform);
}

ViewportCube::~ViewportCube()
{
	main_widget.deleteLater();
}

void ViewportCube::setViewcubePosition(const ivec2 &pos)
{
	main_widget->setPosition(pos.x, pos.y);
}

ivec2 ViewportCube::getViewcubePosition() const
{
	return {main_widget->getPositionX(), main_widget->getPositionY()};
}

void ViewportCube::setChangeProjectionEnabled(bool enabled)
{
	ortho_icon->setEnabled(enabled);
	ortho_icon->setHidden(!enabled);
}

bool ViewportCube::getChangeProjectionEnabled() const
{
	return ortho_icon->isEnabled();
}

// Map a surface name to a direction vector.
// The surface name may include multiple axis tokens (e.g., "px", "py", "pz", "nx", "ny", "nz").
// Resulting vectors represent:
//   - face:    exactly one axis set	(e.g., (1, 0, 0))
//   - edge:    two axes set			(e.g., (1, 1, 0))
//   - corner:  three axes set			(e.g., (1, 1, 1))
// See .../data/cpp_samples/systems/cad_like_view/viewcube/viewcube.mesh file
vec3 ViewportCube::get_polygon_pos(const char *name)
{
	const StringStack<> n = name;
	vec3 pos = vec3_zero;

	if (n.contains("px"))
		pos.x = 1.0f;
	if (n.contains("py"))
		pos.y = 1.0f;
	if (n.contains("pz"))
		pos.z = 1.0f;
	if (n.contains("nx"))
		pos.x = -1.0f;
	if (n.contains("ny"))
		pos.y = -1.0f;
	if (n.contains("nz"))
		pos.z = -1.0f;

	return pos;
}

// Compute a camera rotation that looks toward the given direction
quat ViewportCube::get_polygon_rot(const vec3 &pos)
{
	const vec3 dir = normalize(-pos);
	vec3 up = vec3_up;
	if (dir == vec3_up || dir == vec3_down)
		up = vec3_forward;
	return quat(setTo(vec3_zero, dir, up));
}

// Switch between ortho and perspective projection
void ViewportCube::ortho_toggled()
{
	mat4 projection = player->getProjection();

	const float fov = player->getFov();
	const float znear = player->getZNear();
	const float zfar = player->getZFar();
	const float h = ortho_height;
	const float w = ortho_width;

	ortho_enabled = ortho_icon->isToggled();

	if (ortho_enabled)
	{
		projection = ortho(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f, znear, zfar);
	}
	else
	{
		projection = perspective(fov, 1.0f, znear, zfar);
	}

	player->setProjection(projection);
}

// Start smooth rotation toward a target orientation
void ViewportCube::animate_rotation(const quat &rot)
{
	old_rotation = player->getWorldRotation();
	new_rotation = rot;

	old_position = player->getWorldPosition();
	Scalar distance = (old_position - centering_position).length();
	distance = max(min_distance, distance);
	new_position = centering_position + Vec3(rot.getNormal() * (float)distance);

	timer_rotation = 1.0f;
}

// Update ongoing rotation animation
void ViewportCube::update_rotation()
{
	if (timer_rotation <= 0)
		return;

	timer_rotation -= Game::getIFps() / CAMERA_ANIMATION_PERIOD;

	quat camera_rotation;
	Vec3 camera_position;

	if (timer_rotation > 0)
	{
		camera_rotation = slerp(new_rotation, old_rotation, timer_rotation);
		camera_position = lerp(new_position, old_position, timer_rotation);
	}
	else
	{
		camera_position = new_position;
		camera_rotation = new_rotation;
	}

	player->setWorldTransform(translate(camera_position) * Mat4(camera_rotation));
}

// Main update: handle cube rendering, input, and interactions
void ViewportCube::update()
{
	if (main_widget->isHidden())
		return;

	update_rotation();

	const quat rotation = conjugate(player->getWorldRotation());
	const mat4 projection = player->getProjection();

	const bool is_ortho = (projection.m33 == 1.0f);

	// Sync ortho toggle icon with current projection
	if (!is_hovered && is_ortho != ortho_icon->isToggled())
	{
		MUTE_EVENT(ortho_icon->getEventChanged());
		ortho_icon->setToggled(is_ortho);
	}

	// Apply transforms depending on projection
	if (is_ortho)
	{
		cube_canvas->setTransform(ortho_transform * mat4(rotation));
	}
	else
	{
		cube_canvas->setTransform(perspective_transform * mat4(rotation));
	}

	arrows_canvas->setTransform(ortho_transform * mat4(rotation));

	// Mouse hover detection
	const vec2 mouse_pos((float)cube_canvas->getMouseX(), (float)cube_canvas->getMouseY());
	const vec2 half_size{VIEWCUBE_WIDTH / 2, VIEWCUBE_HEIGHT / 2};
	const float mouse_distance = distance(mouse_pos, half_size);

	is_hovered = checkRange(mouse_pos.x, 0.0f, VIEWCUBE_WIDTH)
		&& checkRange(mouse_pos.y, 0.0f, VIEWCUBE_HEIGHT);

	// Apply fade effect based on cursor distance from cube center
	// Affects the ortho toggle button and navigation arrows
	vec4 color = vec4_one;
	color.w = 1.0f - saturate(mouse_distance / VIEWCUBE_WIDTH / 4.0f - 0.1f);

	if (ortho_hbox->getColor() != color)
	{
		ortho_hbox->setColor(color);

		for (int i = 0, num = arrows_canvas->getNumPolygons(); i < num; ++i)
		{
			arrows_canvas->setPolygonColor(i, color);
		}
	}

	// Highlight cube faces on hover
	int cube_polygon = -1;
	const int cube_num_poly = cube_canvas->getNumPolygons();

	if (is_hovered)
	{
		for (int i = 0; i < cube_num_poly; ++i)
		{
			const vec3 pos = rotation * positions[i];
			const int order = (1 + (int)pos.z) * 100;
			cube_canvas->setPolygonOrder(i, order);
			cube_canvas->setPolygonTexture(i, viewcube_texture_untouch_path);
		}

		cube_polygon = cube_canvas->getPolygonIntersection((int)mouse_pos.x, (int)mouse_pos.y);

		if (cube_polygon != -1 && cube_canvas->getPolygonTexture(cube_polygon) != String(viewcube_texture_touch_path))
		{
			cube_canvas->setPolygonTexture(cube_polygon, viewcube_texture_touch_path);
		}
	}

	// Show arrows only when camera is aligned to axis
	if (color.w <= 0.0f && arrows_canvas->isHidden())
	{
		return;
	}

	int arrows_hidden = 1;

	vec3 dir = normalize(conjugate(rotation) * vec3_up);
	dir.x = Math::abs(dir.x);
	dir.y = Math::abs(dir.y);
	dir.z = Math::abs(dir.z);

	if (length2(dir - vec3_right) < 0.1f)
	{
		arrows_hidden = 0;
	}
	else if (length2(dir - vec3_forward) < 0.1f)
	{
		arrows_hidden = 0;
	}
	else if (length2(dir - vec3_up) < 0.1f)
	{
		arrows_hidden = 0;
	}

	arrows_canvas->setHidden(arrows_hidden != 0);

	int arrow_polygon = -1;

	// Highlight arrows on hover
	if (!arrows_hidden && is_hovered)
	{
		for (int i = 0; i < arrows_canvas->getNumPolygons(); ++i)
		{
			arrows_canvas->setPolygonTexture(i, viewcube_texture_untouch_path);
		}

		arrow_polygon = arrows_canvas->getPolygonIntersection((int)mouse_pos.x, (int)mouse_pos.y);

		if (arrow_polygon != -1
			&& arrows_canvas->getPolygonTexture(arrow_polygon) != String(viewcube_texture_touch_path))
		{
			arrows_canvas->setPolygonTexture(arrow_polygon, viewcube_texture_touch_path);
		}
	}

	// Handle mouse clicks:
	// cube_polygon: rotate the camera toward the selected cube surface (face, edge, or corner depending on the surface hit)
	// arrow_polygon: rotate the camera in the direction indicated by the navigation arrow.
	if (!is_hovered)
	{
		return;
	}

	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON_LEFT))
	{
		if (cube_polygon != -1)
		{
			animate_rotation(rotations[cube_polygon]);
		}

		if (arrow_polygon != -1)
		{
			animate_rotation(rotations[cube_num_poly + arrow_polygon]);
		}

		Gui::getCurrent()->removeFocus();
	}
}
