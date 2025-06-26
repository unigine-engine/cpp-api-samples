#include "SimpleMovementInfo.h"

#include "SimpleMovement.h"

REGISTER_COMPONENT(SimpleMovementInfo);

using namespace Unigine;
using namespace Math;

void SimpleMovementInfo::init()
{
	init_components();

	sample_description_window.createWindow();

	sample_description_window.addFloatParameter("Velocity", "Velocity", 3.f, 1.f, 50.f, [this](float v) {
		simple_movement->velocity = v;
		simple_movement2->velocity = v;
		simple_movement3->velocity = v;
	});

	sample_description_window.addFloatParameter("Angular Velocity", "Angular Velocity", 50.f, 1.f, 100.f,
		[this](float v) {
			simple_movement->angular_velocity = v;
			simple_movement2->angular_velocity = v;
			simple_movement3->angular_velocity = v;
		});

	auto parameters = sample_description_window.getParameterGroupBox();
	auto reset_button = WidgetButton::create("Reset Position");
	reset_button->getEventClicked().connect(*this, [this](const Ptr<Widget> &, int) {
		simple_movement_node->setTransform(simple_movement_start_transform);
		simple_movement2_node->setTransform(simple_movement2_start_transform);
		simple_movement3_node->setTransform(simple_movement3_start_transform);
	});
	parameters->addChild(reset_button, Gui::ALIGN_LEFT | Gui::ALIGN_EXPAND);
}

void SimpleMovementInfo::shutdown()
{
	sample_description_window.shutdown();
}

void SimpleMovementInfo::init_components()
{
	simple_movement = getComponent<SimpleMovement>(simple_movement_node);
	simple_movement2 = getComponent<SimpleMovement2>(simple_movement2_node);
	simple_movement3 = getComponent<SimpleMovement3>(simple_movement3_node);

	if (!simple_movement)
	{
		Log::error("SimpleMovementInfo::init: cannot find SimpleMovement node!\n");
	}
	else
	{
		simple_movement_start_transform = simple_movement_node->getTransform();
	}

	if (!simple_movement2)
	{
		Log::error("SimpleMovementInfo::init: cannot find SimpleMovement2 node!\n");
	}
	else
	{
		simple_movement2_start_transform = simple_movement2_node->getTransform();
	}

	if (!simple_movement3)
	{
		Log::error("SimpleMovementInfo::init: cannot find SimpleMovement3 node!\n");
	}
	else
	{
		simple_movement3_start_transform = simple_movement3_node->getTransform();
	}
}
