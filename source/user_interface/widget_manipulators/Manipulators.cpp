#include "Manipulators.h"
#include <UnigineWidgets.h>
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UniginePlayers.h>

REGISTER_COMPONENT(Manipulators);

using namespace Unigine;
using namespace Math;

void get_mesh_bs(const NodePtr& node, WorldBoundSphere& bs)
{
	if (!node) return;

	if (node->isObject())
		bs.expand(node->getWorldBoundSphere());

	if (node->getType() == Node::NODE_REFERENCE)
		get_mesh_bs(checked_ptr_cast<NodeReference>(node)->getReference(), bs);
	for (int i = 0; i < node->getNumChildren(); i++)
		get_mesh_bs(node->getChild(i), bs);
}

void Manipulators::init()
{
	Input::setMouseHandle(Input::MOUSE_HANDLE_GRAB);

	// by default all axes are available for each manipulator
	is_x_axis_rotation = true;
	is_y_axis_rotation = true;
	is_z_axis_rotation = true;
	is_x_axis_translation = true;
	is_y_axis_translation = true;
	is_z_axis_translation = true;
	is_x_axis_scale = true;
	is_y_axis_scale = true;
	is_z_axis_scale = true;

	// by default manipulators work in world basis
	is_local_basis = false;

	// creating manipulators
	gui = Gui::getCurrent();
	object_translator = WidgetManipulatorTranslator::create(gui);
	object_rotator = WidgetManipulatorRotator::create(gui);
	object_scaler = WidgetManipulatorScaler::create(gui);

	gui->addChild(object_translator);
	gui->addChild(object_rotator);
	gui->addChild(object_scaler);

	object_translator->setHidden(true);
	object_rotator->setHidden(true);
	object_scaler->setHidden(true);
	current_object_manipulator = object_translator;

	object_translator->getEventChanged().connect(this, &Manipulators::apply_transform);
	object_rotator->getEventChanged().connect(this, &Manipulators::apply_transform);
	object_scaler->getEventChanged().connect(this, &Manipulators::apply_transform);

	auto player = Game::getPlayer();
	player->setControlled(false);
}

void Manipulators::update()
{
	auto player = Game::getPlayer();

	// if mouse is grabbed, we enable player control making hotkeys for switching manipulators unavailable
	if (Input::isMouseGrab())
		player->setControlled(true);

	// reset projection and view matrices for correct rendering of the widget
	if (player)
	{
		object_translator->setProjection(player->getProjection());
		object_rotator->setProjection(player->getProjection());
		object_scaler->setProjection(player->getProjection());

		object_translator->setModelview(player->getCamera()->getModelview());
		object_rotator->setModelview(player->getCamera()->getModelview());
		object_scaler->setModelview(player->getCamera()->getModelview());
	}

	// trying to get an object to manipulate
	if (Input::isMouseButtonDown(Input::MOUSE_BUTTON::MOUSE_BUTTON_RIGHT) && !Input::isMouseGrab())
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

	// if some object is selected and it is not a ComponentLogic node
	if (obj)
	{
		// reset manipulator's transform matrix, after moving the object
		if (Input::isMouseButtonUp(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT))
		{
			switch_manipulator(current_object_manipulator);
		}

		// if mouse is not grabbed, disable player control and enable hotkeys to switch manipulators
		if (!Input::isMouseGrab())
		{
			player->setControlled(false);

			if (Input::isKeyDown(Input::KEY::KEY_W))
				switch_manipulator(object_translator);

			if (Input::isKeyDown(Input::KEY::KEY_E))
				switch_manipulator(object_rotator);

			if (Input::isKeyDown(Input::KEY::KEY_R))
				switch_manipulator(object_scaler);
		}

		// hotkey to focus on an object at focus_distance distance
		if (Input::isKeyDown(Input::KEY::KEY_F))
		{
			vec3 inversePlayerViewDirection = -player->getViewDirection();
			WorldBoundSphere bs;
			get_mesh_bs(obj, bs);
			player->setWorldPosition(bs.center + Vec3(inversePlayerViewDirection * ((float)bs.radius * 2.0f)));
		}

		// hotkeys to unselect an object
		if (Input::isKeyDown(Input::KEY::KEY_U) || Input::isKeyDown(Input::KEY::KEY_ESC)) // || Input::isMouseCursorHide())
		{
			unselect();
		}
	}
}

void Manipulators::shutdown()
{
	// destroy widgets
	object_translator.deleteLater();
	object_rotator.deleteLater();
	object_scaler.deleteLater();
}

void Manipulators::apply_transform()
{
	// applying transformation of the widget to the object
	if (obj)
	{
		NodePtr manipulate_node = obj;

		if (transform_parent && manipulate_node->getParent())
			manipulate_node = manipulate_node->getParent();

		manipulate_node->setWorldTransform(current_object_manipulator->getTransform());
	}
}

Unigine::ObjectPtr Manipulators::get_node_under_cursor()
{
	auto player = Game::getPlayer();
	ivec2 mouse = Input::getMousePosition();

	// returns the first object intersected by the ray casted from the player in the forward direction at a distance of 10000 units
	return World::getIntersection(player->getWorldPosition(), player->getWorldPosition() + Vec3(player->getDirectionFromMainWindow(mouse.x, mouse.y) * 10000), intersection_mask.get());
}

void Manipulators::switch_manipulator(const Unigine::WidgetManipulatorPtr &currentManipulator)
{
	if (obj)
	{
		set_manipulators_basis();

		current_object_manipulator = currentManipulator;
		current_object_manipulator->setHidden(false);

		NodePtr manipulate_node = obj;

		if (transform_parent && manipulate_node->getParent())
			manipulate_node = manipulate_node->getParent();

		current_object_manipulator->setTransform(manipulate_node->getWorldTransform());


		// show only selected manipulator
		if (object_translator != current_object_manipulator)
			object_translator->setHidden(true);
		if (object_rotator != current_object_manipulator)
			object_rotator->setHidden(true);
		if (object_scaler != current_object_manipulator)
			object_scaler->setHidden(true);
	}
}

void Manipulators::unselect()
{
	// reset selection to the DummyNode(ComponentLogic node in the world)
	obj = ObjectPtr();

	// hide all manipulators
	object_translator->setHidden(true);
	object_rotator->setHidden(true);
	object_scaler->setHidden(true);
}

void Manipulators::set_manipulators_basis()
{
	if (obj)
	{
		if (is_local_basis)
		{
			// reset the basis of manipulators to object's local basis
			object_rotator->setBasis(obj->getWorldTransform());
			object_translator->setBasis(obj->getWorldTransform());
			object_scaler->setBasis(obj->getWorldTransform());
		}
		else
		{
			// resets the basis of manipulators to world basis
			object_rotator->setBasis(Mat4_identity);
			object_translator->setBasis(Mat4_identity);
			object_scaler->setBasis(Mat4_identity);
		}
	}
}

// enables or disables the corresponding axis for each manipulator
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

// switches the basis for manipulators from local to world and back from world to local
void Manipulators::setLocalBasis(bool value)
{
	is_local_basis = value;
	set_manipulators_basis();
}
