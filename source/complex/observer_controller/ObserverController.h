#pragma once

#include <UnigineComponentSystem.h>

class ObserverController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(ObserverController, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_GROUP("Input")
	PROP_PARAM(String, focus_key, "F", "Focusing Key", "Key to focus the camera on the object, zooming in for a closer view.");
	PROP_PARAM(String, toggle_camera_menu, "F3", "Toggle Camera Menu", "Key to toggle the visibility of the camera menu.");
	PROP_PARAM(String, alt_camera_mode, "ANY_ALT", "Camera Control Modifier", "Modifier key for switching between different camera control modes.");
	PROP_PARAM(String, acceleration_key, "ANY_SHIFT", "Acceleration Key", "Key to activate acceleration mode, typically used for faster movement.");
	PROP_PARAM(String, spectator_mode, "RIGHT", "Spectator Mode Mouse Button", "Mouse button to toggle spectator mode.");
	PROP_PARAM(String, rail_mode, "RIGHT", "Rail Mode Mouse Button", "Mouse button to activate rail mode.");
	PROP_PARAM(String, panning_mode, "MIDDLE", "Panning Mode Mouse Button", "Mouse button to initiate panning mode.");
	PROP_PARAM(Float, panning_rail_scale, 0.01f, "Panning Mode Mouse Button", "Mouse button to initiate panning mode.");

	PROP_GROUP("Velocity")
	PROP_PARAM(String, first_gear_key, "DIGIT_1", "First Gear Key", "Key to activate the first gear (low speed).");
	PROP_PARAM(Float, first_gear_velocity, 5.0f, "First Gear Speed", "Velocity for the first gear.");
	PROP_PARAM(String, second_gear_key, "DIGIT_2", "Second Gear Key", "Key to activate the second gear (medium speed).");
	PROP_PARAM(Float, second_gear_velocity, 50.0f, "Second Gear Speed", "Velocity for the second gear.");
	PROP_PARAM(String, third_gear_key, "DIGIT_3", "Third Gear Key", "Key to activate the third gear (high speed).");
	PROP_PARAM(Float, third_gear_velocity, 500.0f, "Third Gear Speed", "Velocity for the third gear.");
	PROP_PARAM(Float, accelearation_multiplier, 2.0f, "Acceleration Multiplier", "Multiplier for acceleration when the assigned key is held.");

private:
	const float DEFAULT_VELOCITY = 5.0f;
	const float DEFAULT_POSITION_VALUE = 0;

	bool edit_text = false;
	bool try_end_focusing = false;
	bool enter_mouse_grab_mode = false;

	Unigine::WidgetHBoxPtr menu_layout = nullptr;
	Unigine::WidgetCheckBoxPtr first_gear_checkbox = nullptr;
	Unigine::WidgetCheckBoxPtr second_gear_checkbox = nullptr;
	Unigine::WidgetCheckBoxPtr third_gear_checkbox = nullptr;

	Unigine::Vector<Unigine::WidgetEditLinePtr> edit_lines;

	Unigine::Math::Vec3 target_point;

	Unigine::Input::KEY _toggle_camera_menu = Unigine::Input::KEY::KEY_UNKNOWN;
	Unigine::Input::KEY _focus_key = Unigine::Input::KEY::KEY_UNKNOWN;
	Unigine::Input::KEY _acceleration_key = Unigine::Input::KEY::KEY_UNKNOWN;
	Unigine::Input::KEY _first_gear_key = Unigine::Input::KEY::KEY_UNKNOWN;
	Unigine::Input::KEY _second_gear_key = Unigine::Input::KEY::KEY_UNKNOWN;
	Unigine::Input::KEY _third_gear_key = Unigine::Input::KEY::KEY_UNKNOWN;

	Unigine::Input::MOUSE_BUTTON _spectator_mode = Unigine::Input::MOUSE_BUTTON::MOUSE_BUTTON_UNKNOWN;
	Unigine::Input::MOUSE_BUTTON _rail_mode = Unigine::Input::MOUSE_BUTTON::MOUSE_BUTTON_UNKNOWN;
	Unigine::Input::MOUSE_BUTTON _panning_mode = Unigine::Input::MOUSE_BUTTON::MOUSE_BUTTON_UNKNOWN;

	Unigine::Input::MODIFIER _alt_camera_mode = Unigine::Input::MODIFIER::MODIFIER_NONE;

	Unigine::PlayerSpectatorPtr player_camera = nullptr;

	enum PlayerMovementState
	{
		IDLE,
		SPECTATOR,
		RAIL,
		FOCUSING,
		PANNING
	};

	PlayerMovementState player_state = IDLE;

	enum VelocityGear
	{
		GEAR_FIRST = 1,
		GEAR_SECOND,
		GEAR_THIRD
	};

	VelocityGear velocity_gear = GEAR_FIRST;

	struct StateTransition
	{
		std::function<bool()> condition;
		PlayerMovementState target_state;
	};

	struct MovementState
	{
		Unigine::Vector<StateTransition> transitions;
		std::function<void()> on_enter;
		std::function<void()> on_exit;
		std::function<void()> on_update;
	};

	Unigine::Map<PlayerMovementState, MovementState> state_map;

	inline bool try_focusing() const { return Unigine::Input::isKeyDown(_focus_key); }
	inline bool try_exit_rail_mode() const { return Unigine::Input::isMouseButtonUp(_rail_mode); }
	inline bool try_exit_panning_mode() const { return Unigine::Input::isMouseButtonUp(_panning_mode); }
	inline bool try_enter_spectator_mode() const
	{
		return !Unigine::Input::isModifierEnabled(_alt_camera_mode) && Unigine::Input::isMouseButtonPressed(_spectator_mode);
	}
	inline bool try_enter_rail_mode() const
	{
		return Unigine::Input::isModifierEnabled(_alt_camera_mode) && Unigine::Input::isMouseButtonPressed(_rail_mode);
	}
	inline bool try_enter_panning_mode() const
	{
		return Unigine::Input::isModifierEnabled(_alt_camera_mode) && Unigine::Input::isMouseButtonPressed(_panning_mode);
	}


private:
	void init();
	void init_spectator();
	void init_panning();
	void init_focusing();
	void init_menu();

	void update();
	void update_state();
	void update_gear(VelocityGear new_gear);
	void update_velocity_gear();
	void update_spectator();
	void update_rail();
	void update_panning();
	void update_focusing();
	void update_menu();
	void update_edit_field_submission();

	void shutdown();
	void end_spectator();
	void end_panning();
	void end_focusing();

	void create_state_map();

	void switch_state(PlayerMovementState target_state);

	void assign_inputs();

	void change_gear_text_field(VelocityGear target_gear);

	void set_velocity(VelocityGear target_gear, float velocity);

	float get_velocity() const;
	float get_velocity_acceleration() const;
};
