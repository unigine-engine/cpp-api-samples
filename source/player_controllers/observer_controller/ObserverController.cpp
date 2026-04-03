// Editor-style camera controller with multiple movement modes: spectator
// (free-fly), panning, rail (dolly), and focus-on-object. Uses a state
// machine to handle transitions between modes and supports velocity gears.

#include "ObserverController.h"

#include <UnigineGame.h>
#include <UnigineConsole.h>

REGISTER_COMPONENT(ObserverController);

using namespace Unigine;
using namespace Math;

// Initializes observer: gets PlayerSpectator camera, converts string key names
// to input enums, sets up state machine, and creates GUI menu.
void ObserverController::init()
{
	player_camera = checked_ptr_cast<PlayerSpectator>(Game::getPlayer());
	enter_mouse_grab_mode = Input::isMouseGrab();

	assign_inputs();
	create_state_map();
	update_gear(velocity_gear);
	init_menu();
}

// Enables built-in PlayerSpectator WASD movement and mouse look.
void ObserverController::init_spectator()
{
	player_camera->setControlled(true);
	ControlsApp::setMouseEnabled(true);
}

// Grabs mouse for continuous delta tracking during pan movement.
void ObserverController::init_panning()
{
	Input::setMouseGrab(true);
	ControlsApp::setMouseEnabled(true);
}

// Raycasts from camera through mouse position to find object to focus on.
// Calculates target position at twice the object's bounding radius distance.
void ObserverController::init_focusing()
{
	WorldIntersectionPtr intersection = WorldIntersection::create();

	Vec3 start_point, end_point, obj_position;

	Scalar focus_radius;

	player_camera->getDirectionFromMainWindow(start_point, end_point, Input::getMousePosition().x, Input::getMousePosition().y);

	NodePtr obj = World::getIntersection(start_point, end_point, ~0, intersection);

	if (!obj)
	{
		try_end_focusing = true;
		return;
	}

	obj_position = obj->getWorldPosition();
	focus_radius = obj->getWorldBoundSphere().radius;

	// Block, with an example for focusing specifically on instances of ObjectMeshCluster
	if (ObjectMeshClusterPtr cluster = checked_ptr_cast<ObjectMeshCluster>(obj)) 
	{
		int instance_index = intersection->getInstance();

		ConstMeshPtr mesh = cluster->getMeshCurrentRAM();
		Mat4 transform = Mat4(cluster->getMeshTransform(instance_index));

		obj_position = cluster->getWorldTransform() * transform.getTranslate();

		// When searching the radius, we take into account both the scale of the instance and the ObjectMeshCluster itself.
		focus_radius = mesh->getBoundSphere().radius * cluster->getScale().max() * transform.getScale().max();
	}

	// Position camera at 2x radius distance so object is nicely framed
	target_point = obj_position - Vec3(player_camera->getWorldDirection() * focus_radius * 2);
}

// Creates the top-of-screen menu with speed control (edit field + gear checkboxes)
// and position fields (X, Y, Z) for teleporting camera to specific coordinates.
void ObserverController::init_menu()
{
	menu_layout = WidgetHBox::create(0, 4);
	menu_layout->setBackground(1);
	Gui::getCurrent()->addChild(menu_layout, Gui::ALIGN_TOP);

	WidgetHBoxPtr gears_layout = WidgetHBox::create(5, 0);
	menu_layout->addChild(gears_layout, Gui::ALIGN_LEFT);

	WidgetLabelPtr labelName = WidgetLabel::create("Player speed:");
	gears_layout->addChild(labelName, Gui::ALIGN_LEFT);

	edit_lines.push_back(WidgetEditLine::create(String::ftoa(get_velocity(), 5)));
	edit_lines[0]->setValidator(3);
	edit_lines[0]->setWidth(100);
	gears_layout->addChild(edit_lines[0], Gui::ALIGN_LEFT);

	edit_lines[0]->getEventFocusIn().connect(*this, [this]() { edit_text = true; });
	edit_lines[0]->getEventFocusOut().connect(*this, [this]() {
		edit_text = false;
		set_velocity(
			velocity_gear,
			String::isEmpty(edit_lines[0]->getText()) ? DEFAULT_VELOCITY : String::atof(edit_lines[0]->getText()));
	});

	first_gear_checkbox = WidgetCheckBox::create("1");
	first_gear_checkbox->setChecked(true);
	gears_layout->addChild(first_gear_checkbox, Gui::ALIGN_LEFT);
	first_gear_checkbox->getEventChanged().connect(*this, [this]() {
		if (first_gear_checkbox->isChecked())
			change_gear_text_field(VelocityGear::GEAR_FIRST);
	});

	second_gear_checkbox = WidgetCheckBox::create("2");
	first_gear_checkbox->addAttach(second_gear_checkbox);
	gears_layout->addChild(second_gear_checkbox, Gui::ALIGN_LEFT);
	second_gear_checkbox->getEventChanged().connect(*this, [this]() {
		if (second_gear_checkbox->isChecked())
			change_gear_text_field(VelocityGear::GEAR_SECOND);
	});

	third_gear_checkbox = WidgetCheckBox::create("3");
	first_gear_checkbox->addAttach(third_gear_checkbox);
	gears_layout->addChild(third_gear_checkbox, Gui::ALIGN_LEFT);
	third_gear_checkbox->getEventChanged().connect(*this, [this]() {
		if (third_gear_checkbox->isChecked())
			change_gear_text_field(VelocityGear::GEAR_THIRD);
	});

	WidgetSpacerPtr gear_spacer = WidgetSpacer::create();
	gear_spacer->setOrientation(0);
	menu_layout->addChild(gear_spacer, Gui::ALIGN_LEFT);

	WidgetHBoxPtr position_layout = WidgetHBox::create(5, 0);
	menu_layout->addChild(position_layout, Gui::ALIGN_LEFT);

	labelName = WidgetLabel::create("X:");
	position_layout->addChild(labelName, Gui::ALIGN_LEFT);

	edit_lines.push_back(WidgetEditLine::create(String::dtoa((double)player_camera->getWorldPosition().x)));
	edit_lines[1]->setValidator(3);
	edit_lines[1]->setWidth(100);
	position_layout->addChild(edit_lines[1], Gui::ALIGN_LEFT);
	edit_lines[1]->getEventFocusIn().connect(*this, [this]() { edit_text = true; });
	edit_lines[1]->getEventKeyPressed().connect(*this, [this]() {
		Scalar value = String::isEmpty(edit_lines[1]->getText()) ? DEFAULT_POSITION_VALUE : String::atof(edit_lines[1]->getText());
		Vec3 position = player_camera->getWorldPosition();
		player_camera->setWorldPosition(Vec3(value, position.y, position.z));
	});
	edit_lines[1]->getEventFocusOut().connect(*this, [this]() {
		edit_text = false;
		if (String::isEmpty(edit_lines[1]->getText()))
			edit_lines[1]->setText(String::ftoa(DEFAULT_POSITION_VALUE));
	});

	labelName = WidgetLabel::create("Y:");
	position_layout->addChild(labelName, Gui::ALIGN_LEFT);

	edit_lines.push_back(WidgetEditLine::create(String::dtoa((double)player_camera->getWorldPosition().y)));
	edit_lines[2]->setValidator(3);
	edit_lines[2]->setWidth(100);
	position_layout->addChild(edit_lines[2], Gui::ALIGN_LEFT);
	edit_lines[2]->getEventFocusIn().connect(*this, [this]() { edit_text = true; });
	edit_lines[2]->getEventKeyPressed().connect(*this, [this]() {
		Scalar value = String::isEmpty(edit_lines[2]->getText()) ? DEFAULT_POSITION_VALUE : String::atof(edit_lines[2]->getText());
		Vec3 position = player_camera->getWorldPosition();
		player_camera->setWorldPosition(Vec3(position.x, value, position.z));
	});
	edit_lines[2]->getEventFocusOut().connect(*this, [this]() {
		edit_text = false;
		if (String::isEmpty(edit_lines[2]->getText()))
			edit_lines[2]->setText(String::ftoa(DEFAULT_POSITION_VALUE));
	});

	labelName = WidgetLabel::create("Z:");
	position_layout->addChild(labelName, Gui::ALIGN_LEFT);

	edit_lines.push_back(WidgetEditLine::create(String::dtoa((double)player_camera->getWorldPosition().z)));
	edit_lines[3]->setValidator(3);
	edit_lines[3]->setWidth(100);
	position_layout->addChild(edit_lines[3], Gui::ALIGN_LEFT);
	edit_lines[3]->getEventFocusIn().connect(*this, [this]() { edit_text = true; });
	edit_lines[3]->getEventKeyPressed().connect(*this, [this]() {
		Scalar value = String::isEmpty(edit_lines[3]->getText()) ? DEFAULT_POSITION_VALUE : String::atof(edit_lines[3]->getText());
		Vec3 position = player_camera->getWorldPosition();
		player_camera->setWorldPosition(Vec3(position.x, position.y, value));
	});
	edit_lines[3]->getEventFocusOut().connect(*this, [this]() {
		edit_text = false;
		if (String::isEmpty(edit_lines[3]->getText()))
			edit_lines[3]->setText(String::ftoa(DEFAULT_POSITION_VALUE));
	});
}

// Main update: skips input when console is open or text field is focused,
// otherwise checks gear keys, runs state machine, and syncs menu.
void ObserverController::update()
{
	if (Console::isActive())
		return;
	if (edit_text)
	{
		update_edit_field_submission();
		return;
	}
	update_velocity_gear();
	update_state();
	update_menu();
}

// State machine tick: checks all transition conditions for current state,
// switches if any is true, otherwise calls the state's update function.
void ObserverController::update_state()
{
	auto &state = state_map[player_state];

	// Check transitions in priority order (first match wins)
	for (const auto &transition : state.transitions)
	{
		if (transition.condition())
		{
			switch_state(transition.target_state);
			return;
		}
	}

	if (state.on_update)
		state.on_update();
}

// Applies the selected gear's velocity to PlayerSpectator min/max speed.
void ObserverController::update_gear(VelocityGear new_gear)
{
	velocity_gear = new_gear;
	player_camera->setMinVelocity(get_velocity());
	player_camera->setMaxVelocity(get_velocity_acceleration());
}

// Checks number keys 1-3 to switch velocity gears.
void ObserverController::update_velocity_gear()
{
	if (Input::isKeyDown(_first_gear_key))
	{
		first_gear_checkbox->setChecked(true);
	}

	if (Input::isKeyDown(_second_gear_key))
	{
		second_gear_checkbox->setChecked(true);
	}

	if (Input::isKeyDown(_third_gear_key))
	{
		third_gear_checkbox->setChecked(true);
	}
}

// Spectator mode: just hides cursor, actual movement handled by PlayerSpectator.
void ObserverController::update_spectator()
{
	Input::setMouseCursorHide(true);
}

// Rail mode: moves camera forward/backward along view direction based on mouse.
// Combined X+Y mouse delta controls dolly speed (like zooming in/out).
void ObserverController::update_rail()
{
	Input::setMouseCursorHide(true);

	ivec2 mouse_delta = Input::getMouseDeltaPosition();
	normalizeValid(vec2(mouse_delta));

	float current_acceleration = Input::isKeyPressed(_acceleration_key) ? get_velocity() : get_velocity_acceleration();
	float delta = -(mouse_delta.x + mouse_delta.y) * current_acceleration * ControlsApp::getMouseSensitivity() * panning_rail_scale;

	// Translate along local Z (forward) axis
	player_camera->translate(Vec3(0, 0, delta));
}

// Panning mode: moves camera perpendicular to view (left/right, up/down).
void ObserverController::update_panning()
{
	Input::setMouseCursorHide(true);

	ivec2 mouse_delta = Input::getMouseDeltaPosition();
	normalizeValid(vec2(mouse_delta));

	float current_acceleration = Input::isKeyPressed(_acceleration_key) ? get_velocity() : get_velocity_acceleration();
	// Translate along local X (right) and Y (up) axes
	player_camera->translate(Vec3(float(-mouse_delta.x), float(mouse_delta.y), 0.0f) * current_acceleration * ControlsApp::getMouseSensitivity() * panning_rail_scale);
}

// Focus mode: smoothly interpolates camera position toward clicked object.
void ObserverController::update_focusing()
{
	player_camera->setWorldPosition(lerp(player_camera->getWorldPosition(), target_point, Game::getIFps() * 5));
	// Signal completion when close enough to target
	if (distance2(player_camera->getWorldPosition(), target_point) < 0.01f)
		try_end_focusing = true;
}

// Toggles menu visibility with F3, updates position fields with camera coordinates.
void ObserverController::update_menu()
{
	if (Input::isKeyDown(_toggle_camera_menu))
		menu_layout->setHidden(!menu_layout->isHidden());

	// Sync position fields with current camera position
	Vec3 player_position = player_camera->getWorldPosition();
	edit_lines[1]->setText(String::dtoa((double)player_position.x, 5));
	edit_lines[2]->setText(String::dtoa((double)player_position.y, 5));
	edit_lines[3]->setText(String::dtoa((double)player_position.z, 5));
}

// Removes focus from all edit fields when Enter is pressed.
void ObserverController::update_edit_field_submission()
{
	if (!edit_lines.empty() && Input::isKeyDown(Input::KEY_ENTER))
	{
		for (const auto &line : edit_lines)
			line->removeFocus();
	}
}

// Cleans up GUI and restores original mouse grab state.
void ObserverController::shutdown()
{
	menu_layout.deleteLater();
	Input::setMouseGrab(enter_mouse_grab_mode);
}

// Disables PlayerSpectator built-in control when leaving spectator mode.
void ObserverController::end_spectator()
{
	player_camera->setControlled(false);
}

// Releases mouse grab when exiting panning mode.
void ObserverController::end_panning()
{
	Input::setMouseGrab(false);
}

// Resets focus completion flag when exiting focus mode.
void ObserverController::end_focusing()
{
	try_end_focusing = false;
}

// Defines state machine: each state has a list of transition conditions,
// plus optional on_enter, on_exit, and on_update callbacks.
void ObserverController::create_state_map()
{
	// IDLE: waiting state, can transition to any mode based on input
	state_map[PlayerMovementState::IDLE] = {
		{{[this]() { return try_focusing(); }, PlayerMovementState::FOCUSING},
			{[this]() { return try_enter_spectator_mode(); }, PlayerMovementState::SPECTATOR},
			{[this]() { return try_enter_rail_mode(); }, PlayerMovementState::RAIL},
			{[this]() { return try_enter_panning_mode(); }, PlayerMovementState::PANNING}},
		nullptr,
		nullptr,
		nullptr};

	state_map[PlayerMovementState::SPECTATOR] = {
		{{[this]() { return !try_enter_spectator_mode(); }, PlayerMovementState::IDLE}},
		[this]() { init_spectator(); },
		[this]() { end_spectator(); },
		[this]() { update_spectator(); }};

	state_map[PlayerMovementState::RAIL] = {
		{{[this]() { return try_exit_rail_mode(); }, PlayerMovementState::IDLE}},
		nullptr,
		nullptr,
		[this] { update_rail(); }};
	state_map[PlayerMovementState::FOCUSING] = {
		{{[this]() { return try_end_focusing; }, PlayerMovementState::IDLE},
			{[this]() { return try_enter_spectator_mode(); }, PlayerMovementState::SPECTATOR},
			{[this]() { return try_enter_rail_mode(); }, PlayerMovementState::RAIL},
			{[this]() { return try_enter_panning_mode(); }, PlayerMovementState::PANNING}},
		[this]() { init_focusing(); },
		[this]() { end_focusing(); },
		[this]() { update_focusing(); }};

	state_map[PlayerMovementState::PANNING] = {
		{{[this]() { return try_exit_panning_mode(); }, PlayerMovementState::IDLE}},
		[this]() { init_panning(); },
		[this]() { end_panning(); },
		[this]() { update_panning(); }};
}

// Performs state transition: calls exit callback for old state, then enter for new.
void ObserverController::switch_state(PlayerMovementState target_state)
{
	if (player_state == target_state)
		return;

	if (state_map[player_state].on_exit)
		state_map[player_state].on_exit();

	player_state = target_state;

	if (state_map[player_state].on_enter)
		state_map[player_state].on_enter();
}

// Converts string-based key/button names from PROP_PARAMs to Input enums.
void ObserverController::assign_inputs()
{
	_toggle_camera_menu = Input::getKeyByName(toggle_camera_menu.get());
	_focus_key = Input::getKeyByName(focus_key.get());
	_acceleration_key = Input::getKeyByName(acceleration_key.get());
	_first_gear_key = Input::getKeyByName(first_gear_key.get());
	_second_gear_key = Input::getKeyByName(second_gear_key.get());
	_third_gear_key = Input::getKeyByName(third_gear_key.get());

	_spectator_mode = Input::getMouseButtonByName(spectator_mode);
	_rail_mode = Input::getMouseButtonByName(rail_mode);
	_panning_mode = Input::getMouseButtonByName(panning_mode);

	_alt_camera_mode = Input::getModifierByName(alt_camera_mode);
}

// Updates gear and syncs the speed text field in GUI.
void ObserverController::change_gear_text_field(VelocityGear target_gear)
{
	update_gear(target_gear);
	edit_lines[0]->setText(String::ftoa(get_velocity()));
}

// Stores custom velocity for specified gear (set from GUI edit field).
void ObserverController::set_velocity(VelocityGear target_gear, float velocity)
{
	switch (target_gear)
	{
		case VelocityGear::GEAR_FIRST:
			first_gear_velocity = velocity;
			break;
		case VelocityGear::GEAR_SECOND:
			second_gear_velocity = velocity;
			break;
		case VelocityGear::GEAR_THIRD:
			third_gear_velocity = velocity;
	}
	update_gear(target_gear);
}

// Returns the base velocity for the currently selected gear.
float ObserverController::get_velocity() const
{
	switch (velocity_gear)
	{
		case VelocityGear::GEAR_FIRST:
			return first_gear_velocity;
			break;
		case VelocityGear::GEAR_SECOND:
			return second_gear_velocity;
			break;
		case VelocityGear::GEAR_THIRD:
			return third_gear_velocity;
			break;
		default:
			return 0.0f;
	}
}

// Returns velocity with acceleration multiplier applied (used when Shift is held).
float ObserverController::get_velocity_acceleration() const
{
	return get_velocity() * accelearation_multiplier;
}
