// Implements object manipulation using WidgetManipulator widgets (Translator, Rotator,
// Scaler). Objects are selected via ray intersection on mouse click, and transformed
// using keyboard hotkeys (W/E/R). Supports world and local basis modes with per-axis control.

#include "Manipulators.h"
#include <UnigineGame.h>
#include <UniginePlayers.h>
#include <UnigineWidgets.h>
#include <UnigineWorld.h>

REGISTER_COMPONENT(Manipulators);

using namespace Unigine;
using namespace Math;

// Recursively expands bounding sphere to include all mesh objects in hierarchy.
void get_mesh_bs(const NodePtr &node, WorldBoundSphere &bs)
{
	if (!node) return;

	// Include this node's bounds if it's an object
	if (node->isObject())
		bs.expand(node->getWorldBoundSphere());

	// Recurse into NodeReference targets
	if (node->getType() == Node::NODE_REFERENCE)
		get_mesh_bs(checked_ptr_cast<NodeReference>(node)->getReference(), bs);
	// Recurse into all children
	for (int i = 0; i < node->getNumChildren(); i++)
		get_mesh_bs(node->getChild(i), bs);
}

// Syncs manipulator widget position with current object transform.
void Manipulators::updateManipulatorTransform()
{
	if (obj && current_object_manipulator)
	{
		NodePtr manipulate_node = obj;

		// Use parent transform if transform_parent is enabled
		if (transform_parent && manipulate_node->getParent())
			manipulate_node = manipulate_node->getParent();

		current_object_manipulator->setTransform(manipulate_node->getWorldTransform());
	}
}

// All three manipulator widgets are created and added to GUI, initially hidden.
void Manipulators::init()
{
	// By default all axes are available for each manipulator
	is_x_axis_rotation = true;
	is_y_axis_rotation = true;
	is_z_axis_rotation = true;
	is_x_axis_translation = true;
	is_y_axis_translation = true;
	is_z_axis_translation = true;
	is_x_axis_scale = true;
	is_y_axis_scale = true;
	is_z_axis_scale = true;

	// By default manipulators work in world basis
	is_local_basis = false;

	// Create translator (W), rotator (E), and scaler (R) widgets
	gui = Gui::getCurrent();
	object_translator = WidgetManipulatorTranslator::create(gui);
	object_rotator = WidgetManipulatorRotator::create(gui);
	object_scaler = WidgetManipulatorScaler::create(gui);

	gui->addChild(object_translator);
	gui->addChild(object_rotator);
	gui->addChild(object_scaler);

	// All manipulators start hidden until object is selected
	object_translator->setHidden(true);
	object_rotator->setHidden(true);
	object_scaler->setHidden(true);
	// Translator is default manipulator mode
	current_object_manipulator = object_translator;

	// Connect transform change events to apply_transform callback
	object_translator->getEventChanged().connect(this, &Manipulators::apply_transform);
	object_rotator->getEventChanged().connect(this, &Manipulators::apply_transform);
	object_scaler->getEventChanged().connect(this, &Manipulators::apply_transform);

	// Disable player control to allow manipulator interaction
	auto player = Game::getPlayer();
	player->setControlled(false);
}

// Input is processed for object selection, manipulator switching, and camera focus.
void Manipulators::update()
{
	auto player = Game::getPlayer();

	// Enable player control when mouse is grabbed (disables manipulator hotkeys)
	if (Input::isMouseGrab())
		player->setControlled(true);

	// Sync manipulator projection/modelview with current camera
	if (player)
	{
		object_translator->setProjection(player->getProjection());
		object_rotator->setProjection(player->getProjection());
		object_scaler->setProjection(player->getProjection());

		object_translator->setModelview(player->getCamera()->getModelview());
		object_rotator->setModelview(player->getCamera()->getModelview());
		object_scaler->setModelview(player->getCamera()->getModelview());
	}
	// Handle object selection on left mouse click
	if (Input::isMouseButtonUp(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT) && !Input::isMouseGrab())
	{
		auto hoveredWidget = Gui::getCurrent()->getUnderCursorWidget();
		// Only raycast if not clicking on a widget
		if (!hoveredWidget)
		{
			obj = get_node_under_cursor();
			if (obj)
			{
				switch_manipulator(current_object_manipulator);
			}
			else
			{
				unselect();
			}
		}
	}

	// Process input only when object is selected
	if (obj)
	{
		// Refresh manipulator position after drag operation completes
		if (Input::isMouseButtonUp(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT))
		{
			switch_manipulator(current_object_manipulator);
		}

		// Handle manipulator mode hotkeys when mouse is not grabbed
		if (!Input::isMouseGrab())
		{
			if (Input::isKeyDown(Input::KEY::KEY_W))
				switch_manipulator(object_translator);

			if (Input::isKeyDown(Input::KEY::KEY_E))
				switch_manipulator(object_rotator);

			if (Input::isKeyDown(Input::KEY::KEY_R))
				switch_manipulator(object_scaler);
		}

		// Focus camera on selected object (F key)
		if (Input::isKeyDown(Input::KEY::KEY_F))
		{
			vec3 inversePlayerViewDirection = -player->getViewDirection();
			WorldBoundSphere bs;
			get_mesh_bs(obj, bs);
			// Position camera at 2x bounding sphere radius distance
			player->setWorldPosition(bs.center + Vec3(inversePlayerViewDirection * ((float)bs.radius * 2.0f)));
		}

		// Deselect object (U or Escape key)
		if (Input::isKeyDown(Input::KEY::KEY_U) || Input::isKeyDown(Input::KEY::KEY_ESC))
		{
			unselect();
		}
	}
}

// All manipulator widgets are released.
void Manipulators::shutdown()
{
	object_translator.deleteLater();
	object_rotator.deleteLater();
	object_scaler.deleteLater();
}

// Manipulator widget transform is applied to selected object.
void Manipulators::apply_transform()
{
	if (obj)
	{
		NodePtr manipulate_node = obj;

		// Apply to parent if transform_parent mode is enabled
		if (transform_parent && manipulate_node->getParent())
			manipulate_node = manipulate_node->getParent();

		manipulate_node->setWorldTransform(current_object_manipulator->getTransform());
		// Notify listeners of transform change
		transform_changed.run(obj);
	}
}

// Raycasts from camera through mouse position to find selectable object.
Unigine::ObjectPtr Manipulators::get_node_under_cursor()
{
	auto player = Game::getPlayer();
	ivec2 mouse = Input::getMousePosition();

	// Cast ray from camera through mouse position, max distance 10000 units
	return World::getIntersection(player->getWorldPosition(), player->getWorldPosition() + Vec3(player->getDirectionFromMainWindow(mouse.x, mouse.y) * 10000), intersection_mask.get());
}

// Activates specified manipulator, hides others, and syncs transform.
void Manipulators::switch_manipulator(const Unigine::WidgetManipulatorPtr &currentManipulator)
{
	if (obj)
	{
		// Apply current basis mode to all manipulators
		set_manipulators_basis();

		current_object_manipulator = currentManipulator;
		current_object_manipulator->setHidden(false);

		NodePtr manipulate_node = obj;

		if (transform_parent && manipulate_node->getParent())
			manipulate_node = manipulate_node->getParent();

		// Sync manipulator position with object
		current_object_manipulator->setTransform(manipulate_node->getWorldTransform());

		// Hide all other manipulators (only one visible at a time)
		if (object_translator != current_object_manipulator)
			object_translator->setHidden(true);
		if (object_rotator != current_object_manipulator)
			object_rotator->setHidden(true);
		if (object_scaler != current_object_manipulator)
			object_scaler->setHidden(true);
	}
}

// Selection is cleared and all manipulators are hidden.
void Manipulators::unselect()
{
	obj = ObjectPtr();

	object_translator->setHidden(true);
	object_rotator->setHidden(true);
	object_scaler->setHidden(true);
}

// Manipulator coordinate basis is set to local or world mode.
void Manipulators::set_manipulators_basis()
{
	if (obj)
	{
		if (is_local_basis)
		{
			// Use object's local coordinate system for axis orientation
			object_rotator->setBasis(obj->getWorldTransform());
			object_translator->setBasis(obj->getWorldTransform());
			object_scaler->setBasis(obj->getWorldTransform());
		}
		else
		{
			// Use world coordinate system (identity matrix)
			object_rotator->setBasis(Mat4_identity);
			object_translator->setBasis(Mat4_identity);
			object_scaler->setBasis(Mat4_identity);
		}
	}
}

// Enables or disables the corresponding axis for each manipulator
void Manipulators::setXAxisRotation(bool value)
{
	is_x_axis_rotation = value;
	if (is_x_axis_rotation)
		object_rotator->setMask(object_rotator->getMask() | WidgetManipulator::MASK_X);
	else
		object_rotator->setMask(object_rotator->getMask() & ~(WidgetManipulator::MASK_X));
}

void Manipulators::setYAxisRotation(bool value)
{
	is_y_axis_rotation = value;
	if (is_y_axis_rotation)
		object_rotator->setMask(object_rotator->getMask() | WidgetManipulator::MASK_Y);
	else
		object_rotator->setMask(object_rotator->getMask() & ~(WidgetManipulator::MASK_Y));
}

void Manipulators::setZAxisRotation(bool value)
{
	is_z_axis_rotation = value;
	if (is_z_axis_rotation)
		object_rotator->setMask(object_rotator->getMask() | WidgetManipulator::MASK_Z);
	else
		object_rotator->setMask(object_rotator->getMask() & ~(WidgetManipulator::MASK_Z));
}

void Manipulators::setAxesRotation(bool value)
{
	is_x_axis_rotation = value;
	is_y_axis_rotation = value;
	is_z_axis_rotation = value;
	if (is_x_axis_rotation)
		object_rotator->setMask(object_rotator->getMask() | WidgetManipulator::MASK_X);
	else
		object_rotator->setMask(object_rotator->getMask() & ~(WidgetManipulator::MASK_X));

	if (is_y_axis_rotation)
		object_rotator->setMask(object_rotator->getMask() | WidgetManipulator::MASK_Y);
	else
		object_rotator->setMask(object_rotator->getMask() & ~(WidgetManipulator::MASK_Y));

	if (is_z_axis_rotation)
		object_rotator->setMask(object_rotator->getMask() | WidgetManipulator::MASK_Z);
	else
		object_rotator->setMask(object_rotator->getMask() & ~(WidgetManipulator::MASK_Z));
}

void Manipulators::setXAxisTranslation(bool value)
{
	is_x_axis_translation = value;
	if (is_x_axis_translation)
		object_translator->setMask(object_translator->getMask() | WidgetManipulator::MASK_X);
	else
		object_translator->setMask(object_translator->getMask() & ~(WidgetManipulator::MASK_X));
}

void Manipulators::setYAxisTranslation(bool value)
{
	is_y_axis_translation = value;
	if (is_y_axis_translation)
		object_translator->setMask(object_translator->getMask() | WidgetManipulator::MASK_Y);
	else
		object_translator->setMask(object_translator->getMask() & ~(WidgetManipulator::MASK_Y));
}

void Manipulators::setZAxisTranslation(bool value)
{
	is_z_axis_translation = value;
	if (is_z_axis_translation)
		object_translator->setMask(object_translator->getMask() | WidgetManipulator::MASK_Z);
	else
		object_translator->setMask(object_translator->getMask() & ~(WidgetManipulator::MASK_Z));
}

void Manipulators::setAxesTranslation(bool value)
{
	is_x_axis_translation = value;
	is_y_axis_translation = value;
	is_z_axis_translation = value;
	if (is_x_axis_translation)
		object_translator->setMask(object_translator->getMask() | WidgetManipulator::MASK_X);
	else
		object_translator->setMask(object_translator->getMask() & ~(WidgetManipulator::MASK_X));

	if (is_y_axis_translation)
		object_translator->setMask(object_translator->getMask() | WidgetManipulator::MASK_Y);
	else
		object_translator->setMask(object_translator->getMask() & ~(WidgetManipulator::MASK_Y));

	if (is_z_axis_translation)
		object_translator->setMask(object_translator->getMask() | WidgetManipulator::MASK_Z);
	else
		object_translator->setMask(object_translator->getMask() & ~(WidgetManipulator::MASK_Z));
}

void Manipulators::setXAxisScale(bool value)
{
	is_x_axis_scale = value;
	if (is_x_axis_scale)
		object_scaler->setMask(object_scaler->getMask() | WidgetManipulator::MASK_X);
	else
		object_scaler->setMask(object_scaler->getMask() & ~(WidgetManipulator::MASK_X));
}

void Manipulators::setYAxisScale(bool value)
{
	is_y_axis_scale = value;
	if (is_y_axis_scale)
		object_scaler->setMask(object_scaler->getMask() | WidgetManipulator::MASK_Y);
	else
		object_scaler->setMask(object_scaler->getMask() & ~(WidgetManipulator::MASK_Y));
}

void Manipulators::setZAxisScale(bool value)
{
	is_z_axis_scale = value;
	if (is_z_axis_scale)
		object_scaler->setMask(object_scaler->getMask() | WidgetManipulator::MASK_Z);
	else
		object_scaler->setMask(object_scaler->getMask() & ~(WidgetManipulator::MASK_Z));
}

void Manipulators::setAxesScale(bool value)
{
	is_x_axis_scale = value;
	is_y_axis_scale = value;
	is_z_axis_scale = value;
	if (is_x_axis_scale)
		object_scaler->setMask(object_scaler->getMask() | WidgetManipulator::MASK_X);
	else
		object_scaler->setMask(object_scaler->getMask() & ~(WidgetManipulator::MASK_X));

	if (is_y_axis_scale)
		object_scaler->setMask(object_scaler->getMask() | WidgetManipulator::MASK_Y);
	else
		object_scaler->setMask(object_scaler->getMask() & ~(WidgetManipulator::MASK_Y));

	if (is_z_axis_scale)
		object_scaler->setMask(object_scaler->getMask() | WidgetManipulator::MASK_Z);
	else
		object_scaler->setMask(object_scaler->getMask() & ~(WidgetManipulator::MASK_Z));
}

// Switches manipulator basis between local (object) and world coordinate systems
void Manipulators::setLocalBasis(bool value)
{
	is_local_basis = value;
	set_manipulators_basis();
}
