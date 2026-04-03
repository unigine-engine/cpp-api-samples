// Editor-style free camera controller replicating UnigineEditor camera behavior.
// Supports multiple movement modes: spectator (free-fly), panning, rail (dolly
// along view direction), and focus-on-object. Uses a state machine for transitions.

#pragma once

#include <UnigineComponentSystem.h>

// Free-fly camera with editor-like controls: spectator mode, panning, rail zoom,
// object focusing, velocity gears (1-3), and on-screen position/speed menu.
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
	const float DEFAULT_VELOCITY = 5.0f;		// Fallback velocity if field is empty
	const float DEFAULT_POSITION_VALUE = 0;		// Fallback position if field is empty

	bool edit_text = false;				// True when editing text field (disables camera input)
	bool try_end_focusing = false;		// Signals focus animation should complete
	bool enter_mouse_grab_mode = false;	// Original mouse grab state for restoration

	// GUI widgets for speed/position menu
	Unigine::WidgetHBoxPtr menu_layout = nullptr;
	Unigine::WidgetCheckBoxPtr first_gear_checkbox = nullptr;
	Unigine::WidgetCheckBoxPtr second_gear_checkbox = nullptr;
	Unigine::WidgetCheckBoxPtr third_gear_checkbox = nullptr;

	Unigine::Vector<Unigine::WidgetEditLinePtr> edit_lines;	// [0]=speed, [1-3]=XYZ position

	Unigine::Math::Vec3 target_point;	// Destination for focus animation

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

	Unigine::PlayerSpectatorPtr player_camera = nullptr;	// The controlled camera

	// Camera movement modes
	enum PlayerMovementState
	{
		IDLE,		// No active movement, waiting for input
		SPECTATOR,	// Free-fly with mouse look (RMB held)
		RAIL,		// Dolly along view direction (Alt+RMB)
		FOCUSING,	// Animated move toward clicked object
		PANNING		// Strafe perpendicular to view (Alt+MMB)
	};

	PlayerMovementState player_state = IDLE;

	// Speed presets selected by number keys 1-3
	enum VelocityGear
	{
		GEAR_FIRST = 1,		// Slow speed
		GEAR_SECOND,		// Medium speed
		GEAR_THIRD			// Fast speed
	};

	VelocityGear velocity_gear = GEAR_FIRST;

	// Condition and target for state machine transitions
	struct StateTransition
	{
		std::function<bool()> condition;	// Predicate to check each frame
		PlayerMovementState target_state;	// State to enter if condition is true
	};

	// State definition with lifecycle callbacks
	struct MovementState
	{
		Unigine::Vector<StateTransition> transitions;	// Possible exits from this state
		std::function<void()> on_enter;		// Called once when entering state
		std::function<void()> on_exit;		// Called once when leaving state
		std::function<void()> on_update;	// Called every frame while in state
	};

	Unigine::Map<PlayerMovementState, MovementState> state_map;	// State machine definition

	// State transition condition helpers
	inline bool try_focusing() const { return Unigine::Input::isKeyDown(_focus_key); }
	inline bool try_exit_rail_mode() const { return Unigine::Input::isMouseButtonUp(_rail_mode); }
	inline bool try_exit_panning_mode() const { return Unigine::Input::isMouseButtonUp(_panning_mode); }
	inline bool try_enter_spectator_mode() const
	{
		// Spectator requires RMB without Alt modifier
		return !Unigine::Input::isModifierEnabled(_alt_camera_mode) && Unigine::Input::isMouseButtonPressed(_spectator_mode);
	}
	inline bool try_enter_rail_mode() const
	{
		// Rail mode requires Alt + RMB
		return Unigine::Input::isModifierEnabled(_alt_camera_mode) && Unigine::Input::isMouseButtonPressed(_rail_mode);
	}
	inline bool try_enter_panning_mode() const
	{
		// Panning requires Alt + MMB
		return Unigine::Input::isModifierEnabled(_alt_camera_mode) && Unigine::Input::isMouseButtonPressed(_panning_mode);
	}


private:
	// Lifecycle methods
	void init();
	void update();
	void shutdown();

	// State-specific initialization
	void init_spectator();	// Enables PlayerSpectator control
	void init_panning();	// Grabs mouse for pan mode
	void init_focusing();	// Raycasts to find focus target
	void init_menu();		// Creates speed/position GUI widgets

	// Per-frame updates for each state
	void update_state();		// Checks transitions, calls current state update
	void update_spectator();	// Hides cursor during free-fly
	void update_rail();			// Moves camera along view direction
	void update_panning();		// Moves camera perpendicular to view
	void update_focusing();		// Lerps camera toward target point
	void update_menu();			// Syncs position fields with camera
	void update_edit_field_submission();	// Handles Enter key in text fields

	// State-specific cleanup
	void end_spectator();	// Disables PlayerSpectator control
	void end_panning();		// Releases mouse grab
	void end_focusing();	// Clears focus completion flag

	// Velocity gear management
	void update_gear(VelocityGear new_gear);	// Applies gear speed to camera
	void update_velocity_gear();				// Checks 1-3 keys for gear switch
	void change_gear_text_field(VelocityGear target_gear);	// Updates GUI checkbox
	void set_velocity(VelocityGear target_gear, float velocity);	// Stores custom speed

	// State machine
	void create_state_map();	// Defines all states and transitions
	void switch_state(PlayerMovementState target_state);	// Handles enter/exit callbacks

	void assign_inputs();	// Converts string key names to enums

	float get_velocity() const;				// Returns current gear's base speed
	float get_velocity_acceleration() const;	// Returns accelerated speed (with multiplier)
};
