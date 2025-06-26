#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInterface.h>

class FirstPersonController : public Unigine::ComponentBase
{

	const char *key_switch_items =
		"KEY_UNKNOWN,"
		"KEY_ESC,"
		"KEY_F1,"
		"KEY_F2,"
		"KEY_F3,"
		"KEY_F4,"
		"KEY_F5,"
		"KEY_F6,"
		"KEY_F7,"
		"KEY_F8,"
		"KEY_F9,"
		"KEY_F10,"
		"KEY_F11,"
		"KEY_F12,"
		"KEY_PRINTSCREEN,"
		"KEY_SCROLL_LOCK,"
		"KEY_PAUSE,"
		"KEY_BACK_QUOTE,"
		"KEY_DIGIT_1,"
		"KEY_DIGIT_2,"
		"KEY_DIGIT_3,"
		"KEY_DIGIT_4,"
		"KEY_DIGIT_5,"
		"KEY_DIGIT_6,"
		"KEY_DIGIT_7,"
		"KEY_DIGIT_8,"
		"KEY_DIGIT_9,"
		"KEY_DIGIT_0,"
		"KEY_MINUS,"
		"KEY_EQUALS,"
		"KEY_BACKSPACE,"
		"KEY_TAB,"
		"KEY_Q,"
		"KEY_W,"
		"KEY_E,"
		"KEY_R,"
		"KEY_T,"
		"KEY_Y,"
		"KEY_U,"
		"KEY_I,"
		"KEY_O,"
		"KEY_P,"
		"KEY_LEFT_BRACKET,"
		"KEY_RIGHT_BRACKET,"
		"KEY_ENTER,"
		"KEY_CAPS_LOCK,"
		"KEY_A,"
		"KEY_S,"
		"KEY_D,"
		"KEY_F,"
		"KEY_G,"
		"KEY_H,"
		"KEY_J,"
		"KEY_K,"
		"KEY_L,"
		"KEY_SEMICOLON,"
		"KEY_QUOTE,"
		"KEY_BACK_SLASH,"
		"KEY_LEFT_SHIFT,"
		"KEY_LESS,"
		"KEY_Z,"
		"KEY_X,"
		"KEY_C,"
		"KEY_V,"
		"KEY_B,"
		"KEY_N,"
		"KEY_M,"
		"KEY_COMMA,"
		"KEY_DOT,"
		"KEY_SLASH,"
		"KEY_RIGHT_SHIFT,"
		"KEY_LEFT_CTRL,"
		"KEY_LEFT_CMD,"
		"KEY_LEFT_ALT,"
		"KEY_SPACE,"
		"KEY_RIGHT_ALT,"
		"KEY_RIGHT_CMD,"
		"KEY_MENU,"
		"KEY_RIGHT_CTRL,"
		"KEY_INSERT,"
		"KEY_DELETE,"
		"KEY_HOME,"
		"KEY_END,"
		"KEY_PGUP,"
		"KEY_PGDOWN,"
		"KEY_UP,"
		"KEY_LEFT,"
		"KEY_DOWN,"
		"KEY_RIGHT,"
		"KEY_NUM_LOCK,"
		"KEY_NUMPAD_DIVIDE,"
		"KEY_NUMPAD_MULTIPLY,"
		"KEY_NUMPAD_MINUS,"
		"KEY_NUMPAD_DIGIT_7,"
		"KEY_NUMPAD_DIGIT_8,"
		"KEY_NUMPAD_DIGIT_9,"
		"KEY_NUMPAD_PLUS,"
		"KEY_NUMPAD_DIGIT_4,"
		"KEY_NUMPAD_DIGIT_5,"
		"KEY_NUMPAD_DIGIT_6,"
		"KEY_NUMPAD_DIGIT_1,"
		"KEY_NUMPAD_DIGIT_2,"
		"KEY_NUMPAD_DIGIT_3,"
		"KEY_NUMPAD_ENTER,"
		"KEY_NUMPAD_DIGIT_0,"
		"KEY_NUMPAD_DOT,"
		"KEY_ANY_SHIFT,"
		"KEY_ANY_CTRL,"
		"KEY_ANY_ALT,"
		"KEY_ANY_CMD,"
		"KEY_ANY_UP,"
		"KEY_ANY_LEFT,"
		"KEY_ANY_DOWN,"
		"KEY_ANY_RIGHT,"
		"KEY_ANY_ENTER,"
		"KEY_ANY_DELETE,"
		"KEY_ANY_INSERT,"
		"KEY_ANY_HOME,"
		"KEY_ANY_END,"
		"KEY_ANY_PGUP,"
		"KEY_ANY_PGDOWN,"
		"KEY_ANY_DIGIT_1,"
		"KEY_ANY_DIGIT_2,"
		"KEY_ANY_DIGIT_3,"
		"KEY_ANY_DIGIT_4,"
		"KEY_ANY_DIGIT_5,"
		"KEY_ANY_DIGIT_6,"
		"KEY_ANY_DIGIT_7,"
		"KEY_ANY_DIGIT_8,"
		"KEY_ANY_DIGIT_9,"
		"KEY_ANY_DIGIT_0,"
		"KEY_ANY_MINUS,"
		"KEY_ANY_EQUALS,"
		"KEY_ANY_DOT,"
		"NUM_KEYS";
public:
	COMPONENT(FirstPersonController, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_UPDATE_PHYSICS(update_physics);

	PROP_NAME("FirstPersonController");
	PROP_AUTOSAVE(1);

#pragma region EditorParameters

	PROP_PARAM(Switch, forward_key, Unigine::Input::KEY_W, key_switch_items, "Forward Key", "Move forward key", "Input");
	PROP_PARAM(Switch, backward_key, Unigine::Input::KEY_S, key_switch_items, "Backward Key", "Move backward key", "Input");
	PROP_PARAM(Switch, right_key, Unigine::Input::KEY_D, key_switch_items, "Right Key", "Move right key", "Input");
	PROP_PARAM(Switch, left_key, Unigine::Input::KEY_A, key_switch_items, "Left Key", "Move left key", "Input");
	PROP_PARAM(Switch, run_key, Unigine::Input::KEY_ANY_SHIFT, key_switch_items, "Run Key", "Run mode activation key", "Input");
	PROP_PARAM(Switch, jump_key, Unigine::Input::KEY_SPACE, key_switch_items, "Jump Key", "Jump key", "Input");
	PROP_PARAM(Switch, crouch_key, Unigine::Input::KEY_ANY_CTRL, key_switch_items, "Crouch Key", "Crouch mode activation key", "Input");
	PROP_PARAM(Float, mouse_sensitivity, 0.4f, "Mouse Sensitivity", "Mouse sensitivity multiplier", "Input");

	enum GamepadStickSide
	{
		LEFT = 0,
		RIGHT
	};

	PROP_PARAM(Switch, move_stick, 0, "LEFT,RIGHT", "Move Stick", "Move Stick", "Gamepad Input");
	PROP_PARAM(Switch, camera_stick, 1, "LEFT,RIGHT", "Camera Stick", "Camera Stick", "Gamepad Input");
	PROP_PARAM(Float, camera_stick_sensitivity, 0.8f, "Camera stick sensitivity", "Camera stick sensitivity", "Gamepad Input");
	PROP_PARAM(Float, sticks_dead_zone, 0.3f, "Sticks Dead Zone", "Sticks Dead Zone", "Gamepad Input");
	PROP_PARAM(Switch, run_button, Unigine::Input::GAMEPAD_BUTTON_SHOULDER_RIGHT, "BUTTON_A,BUTTON_B,BUTTON_X,BUTTON_Y,BUTTON_BACK,BUTTON_START,BUTTON_DPAD_UP,BUTTON_DPAD_DOWN,BUTTON_DPAD_LEFT,BUTTON_DPAD_RIGHT,BUTTON_THUMB_LEFT,BUTTON_THUMB_RIGHT,BUTTON_SHOULDER_LEFT,BUTTON_SHOULDER_RIGHT", "Run Button", "Run mode activation button", "Gamepad Input");
	PROP_PARAM(Switch, jump_button, Unigine::Input::GAMEPAD_BUTTON_A, "BUTTON_A,BUTTON_B,BUTTON_X,BUTTON_Y,BUTTON_BACK,BUTTON_START,BUTTON_DPAD_UP,BUTTON_DPAD_DOWN,BUTTON_DPAD_LEFT,BUTTON_DPAD_RIGHT,BUTTON_THUMB_LEFT,BUTTON_THUMB_RIGHT,BUTTON_SHOULDER_LEFT,BUTTON_SHOULDER_RIGHT", "Jump Button", "Jump Button", "Gamepad Input");
	PROP_PARAM(Switch, crouch_button, Unigine::Input::GAMEPAD_BUTTON_SHOULDER_LEFT, "BUTTON_A,BUTTON_B,BUTTON_X,BUTTON_Y,BUTTON_BACK,BUTTON_START,BUTTON_DPAD_UP,BUTTON_DPAD_DOWN,BUTTON_DPAD_LEFT,BUTTON_DPAD_RIGHT,BUTTON_THUMB_LEFT,BUTTON_THUMB_RIGHT,BUTTON_SHOULDER_LEFT,BUTTON_SHOULDER_RIGHT", "Crouch Button", "Crouch mode activation button", "Gamepad Input");

	PROP_PARAM(Toggle, use_object_body, 0, "Use Object Body", "You can use current node to customize the body.\nIt should have a dummy body and a capsule shape assigned.\n In case of incorrect settings of the specified custom body or shape, a default body and capsule shape shall be created instead.", "Body");
	PROP_PARAM(Float, capsule_radius, 0.3f, "Capsule Radius", "Radius of the capsule shape", "Body")
	PROP_PARAM(Float, capsule_height, 1.15f, "Capsule Height", "Height of the capsule shape (cylindrical part only)", "Body")
	PROP_PARAM(Mask, physics_intersection_mask, "physics_intersection", 1, "Physics Intersection Mask", "Mask used for selective detections of physics intersections (between physical objects with bodies and collider shapes, or ray intersections with collider geometry)", "Body");
	PROP_PARAM(Mask, collision_mask, "collision", 1, "Collision Mask", "Mask used for selective collision detection", "Body");
	PROP_PARAM(Mask, exclusion_mask, "exclusion", 1, "Exclusion Mask", "Mask used for exclusion collision detection", "Body");

	enum CameraMode
	{
		NONE = 0,
		CREATE_AUTOMATICALLY,
		USE_EXTERNAL
	};

	PROP_PARAM(Switch, camera_mode, 1, "NONE,CREATE_AUTOMATICALLY,USE_EXTERNAL", "Camera Mode", "Camera mode", "Camera");
	PROP_PARAM(Node, camera_editor_node, "Camera", "Camera", "Camera");
	Unigine::PlayerDummyPtr camera;
	PROP_PARAM(Float, fov, 60.0f, "Fov", "Fov", "Camera");
	PROP_PARAM(Float, near_clipping, 0.01f, "Near Clipping", "Near Clipping", "Camera");
	PROP_PARAM(Float, far_clipping, 20000.0f, "Far Clipping", "Far Clipping", "Camera");
	PROP_PARAM(Vec3, camera_position_offset, Unigine::Math::vec3(0.0f, 0.0f, 1.65f), "Camera Position Offset", "Camera position offset", "Camera");
	PROP_PARAM(Float, min_vertical_angle, -89.9f, "Min Vertical Angle", "The minimum vertical angle of the camera (look down)", "Camera");
	PROP_PARAM(Float, max_vertical_angle, 89.9f, "Max Vertical Angle", "The maximum vertical angle of the camera (look up)", "Camera");

	PROP_PARAM(Toggle, use_jump, 1, "Use Jump", "Use jump", "Movement");
	PROP_PARAM(Toggle, use_crouch, 1, "Use Crouch", "Use crouch", "Movement");
	PROP_PARAM(Toggle, use_run, 1, "Use Run", "Use run", "Movement");
	PROP_PARAM(Toggle, use_run_default, 0, "Use Run Default", "Use run default", "Movement");
	PROP_PARAM(Float, crouch_speed, 1.0f, "Crouch Speed", "Crouching speed of the player", "Movement");
	PROP_PARAM(Float, walk_speed, 3.0f, "Walk Speed", "Walking speed of the player", "Movement");
	PROP_PARAM(Float, run_speed, 6.0f, "Run Speed", "Running speed of the player", "Movement");
	PROP_PARAM(Float, ground_acceleration, 25.0f, "Ground Acceleration", "Horizontal ground acceleration of the player", "Movement");
	PROP_PARAM(Float, air_acceleration, 10.0f, "Air Acceleration", "Horizontal air acceleration of the player", "Movement");
	PROP_PARAM(Float, ground_damping, 25.0f, "Ground Damping", "Horizontal ground damping of the player's speed", "Movement");
	PROP_PARAM(Float, air_damping, 25.0f, "Air Damping", "Horizontal air damping of the player's speed", "Movement");
	PROP_PARAM(Float, jump_power, 6.0f, "Jump Power", "Jumping power of the player", "Movement");
	PROP_PARAM(Float, crouch_jump_power, 3.0f, "Crouch Jump Power", "Jumping crouch power of the player", "Movement");
	PROP_PARAM(Float, crouch_height, 1.0f, "Crouch Height", "Crouching height of the player", "Movement");
	PROP_PARAM(Float, crouch_transition_time, 0.3f, "Crouch Transition Time", "Time the player transition to the crouching state and vice versa", "Movement");
	PROP_PARAM(Float, max_ground_angle, 60.0f, "Max Ground Angle", "Maximum surface angle to which the player is considered to be standing on the ground", "Movement");
	PROP_PARAM(Float, max_ceiling_angle, 60.0f, "Max Ceiling Angle", "Maximum surface angle to which the player is considered to be touching the ceiling", "Movement");
	PROP_PARAM(Float, check_move_ray_offset, 0.01f, "Check Move Ray Offset", "Check move ray offset", "Movement");
	PROP_PARAM(Mask, check_move_mask, "intersection", 1, "Check Move Mask", "Check move mask", "Movement");
	PROP_PARAM(Float, wall_stop_sliding_angle, 15.0f, "Wall Stop Sliding Angle", "Wall stop sliding angle", "Movement");

	PROP_PARAM(Toggle, use_auto_stepping, 1, "Use Auto Stepping", "Toggles automatic walking up and down certain obstacles (stairs, for example)", "Auto Stepping");
	PROP_PARAM(Float, min_step_height, 0.05f, "Min Step Height", "Minimum height for automatic walk up.\nEnables avoiding false positives on flat surfaces.", "Auto Stepping");
	PROP_PARAM(Float, max_step_height, 0.3f, "Max Step Height", "Maximum height for automatic walk up.", "Auto Stepping");
	PROP_PARAM(Float, max_stair_angle, 0.3f, "Max Stair Angle", "Max stair angle", "Auto Stepping");
	PROP_PARAM(Float, check_stair_ray_offset, 0.1f, "Check Stair Ray Offset", "Check stair ray offset", "Auto Stepping");
	PROP_PARAM(Float, stairs_detection_angle, 45.0f, "Stairs Detection Angle", "Stairs detection angle", "Auto Stepping");
	PROP_PARAM(Mask, stair_detection_mask, "intersection", 1, "Stair Detection Mask", "Stair detection mask", "Auto Stepping");

	PROP_PARAM(Toggle, use_objects_interaction, 1, "Use Objects Interaction", "Toggles physical interaction with rigid bodies on anf off", "Objects Interaction");
	PROP_PARAM(Float, player_mass, 60.0f, "Player Mass", "Player mass", "Objects Interaction");
	PROP_PARAM(Float, impulse_multiplier, 0.05f, "Impulse Multiplier", "Impulse multiplier to be applied to a rigid body colliding with the player", "Objects Interaction");
	PROP_PARAM(Toggle, ignore_top_contacts, 0, "Ignore Top Contacts", "Ignore top contacts", "Objects Interaction");

	PROP_PARAM(Int, player_fps, 60, "Player Fps", "Minimum update rate for the player (in number of frames per second).\n If this value exceeds the current framerate, the player will be updated several times per frame", "Advanced Settings");
	PROP_PARAM(Int, collision_iterations, 4, "Collision Iterations", "Number of iterations to resolve collision", "Advanced Settings");
	PROP_PARAM(Int, contacts_buffer_size, 16, "Contacts Buffer Size", "Maximum number of contacts to be processed for collisions", "Advanced Settings");
	PROP_PARAM(Int, heavy_contacts_count, 100, "Heavy Contacts Count", "Maximum number of contacts up to which the number of iterations specified above shall be used.\n After exceeding this value a single iteration shall be used to avoid performance degradation.", "Advanced Settings");

#if DEBUG
	struct DebugCamera : public Unigine::ComponentStruct
	{
		PROP_PARAM(Toggle, enabled, 0, "Enabled", "Debug camera enabled");
		PROP_PARAM(Toggle, use_fixed_angles, 0, "Use Fixed Angles", "Use fixed angles");

		Unigine::PlayerDummyPtr camera;
		float angular_speed;
		float zoom_speed;
		float max_distance;
		float horizontal_angle;
		float vertical_angle;
		float distance;
	};

	PROP_STRUCT(DebugCamera, debug_camera, "Debug Camera", "Debug camera", "Debug");

	struct DebugVisualizer : public Unigine::ComponentStruct
	{
		PROP_PARAM(Toggle, enabled, 0, "Enabled", "Use visualizer");
		PROP_PARAM(Toggle, triangles, 0, "Triangles", "Triangles");
		PROP_PARAM(Toggle, shapes, 0, "Shapes", "Shapes");
		PROP_PARAM(Toggle, player_shape, 0, "Player Shape", "Player shape");
		PROP_PARAM(Toggle, player_direction, 0, "Player Direction", "Player direction");
		PROP_PARAM(Toggle, camera, 0, "Camera", "Camera");
		PROP_PARAM(Toggle, slope_basis, 0, "Slope Basis", "Slope basis");
		PROP_PARAM(Toggle, applied_horizontal_velocity, 0, "Applied Horizontal Velocity", "Applied horizontal velocity");
		PROP_PARAM(Toggle, applied_vertical_velocity, 0, "Applied Vertical Velocity", "Applied vertical velocity");
		PROP_PARAM(Toggle, up_pass_contacts, 0, "Up Pass Contacts", "Up pass contacts");
		PROP_PARAM(Toggle, side_pass_contacts, 0, "Side Pass Contacts", "Side pass contacts");
		PROP_PARAM(Toggle, down_pass_contacts, 0, "Down Pass Contacts", "Down pass contacts");
		PROP_PARAM(Toggle, check_move_ray, 0, "Check Move Ray", "Check move ray");
		PROP_PARAM(Toggle, stair_detection_ray, 0, "Stair Detection Ray", "Stair detection ray");
	};

	PROP_STRUCT(DebugVisualizer, debug_visualizer, "Debug Visualizer", "Debug visualizer", "Debug");

	struct DebugProfiler : public Unigine::ComponentStruct
	{
		PROP_PARAM(Toggle, enabled, 0, "Enabled", "Use profiler");
		PROP_PARAM(Toggle, applied_horizontal_speed, 0, "Applied Horizontal Speed", "Applied horizontal speed");
		PROP_PARAM(Toggle, applied_vertical_speed, 0, "Applied Vertical Speed", "Applied vertical speed");
		PROP_PARAM(Toggle, up_pass_contact, 0, "Up Pass Contact", "Up pass contact");
		PROP_PARAM(Toggle, side_pass_contact, 0, "Side Pass Contact", "Side pass contact");
		PROP_PARAM(Toggle, down_pass_contact, 0, "Down Pass Contact", "Down pass contact");
		PROP_PARAM(Toggle, is_ground, 0, "Is Ground", "Is ground");
		PROP_PARAM(Toggle, is_ceiling, 0, "Is Ceiling", "Is ceiling");
		PROP_PARAM(Toggle, is_crouch, 0, "Is Crouch", "Is crouch");
		PROP_PARAM(Toggle, average_speed, 0, "Average Speed", "Average speed");
		PROP_PARAM(Toggle, auto_stepping, 0, "Auto Stepping", "Auto stepping");
	};

	PROP_STRUCT(DebugProfiler, debug_profiler, "Debug Profiler", "Debug profiler", "Debug");

	struct DebugColors : Unigine::ComponentStruct
	{
		PROP_PARAM(Color, player_shape, Unigine::Math::vec4(0.0f, 0.0f, 1.0f, 0.098f), "Player Shape", "Player shape");
		PROP_PARAM(Color, player_direction, Unigine::Math::vec4(1.0f, 1.0f, 0.0f, 1.0f), "Player Direction", "Player direction"); // yellow
		PROP_PARAM(Color, camera_color, Unigine::Math::vec4(0.0f, 1.0f, 1.0f, 1.0f), "Camera Color", "Camera color"); // cyan
		PROP_PARAM(Color, applied_horizontal_velocity, Unigine::Math::vec4_black, "Applied Horizontal Velocity", "Applied horizontal velocity");
		PROP_PARAM(Color, applied_vertical_velocity, Unigine::Math::vec4(0.7f, 0.75f, 0.71f, 1.0f), "Applied Vertical Velocity", "Applied vertical velocity");
		PROP_PARAM(Color, up_pass_contacts, Unigine::Math::vec4(0.0f, 1.0f, 1.0f, 1.0f), "Up Pass Contacts", "Up pass contacts"); // cyan
		PROP_PARAM(Color, side_pass_contacts, Unigine::Math::vec4(0.87f, 0.0f, 1.0f, 1.0f), "Side Pass Contacts", "Side pass contacts"); // magneta
		PROP_PARAM(Color, down_pass_contacts, Unigine::Math::vec4_red, "Down Pass Contacts", "Down pass contacts");
		PROP_PARAM(Color, is_ground, Unigine::Math::vec4_red, "Is Ground", "Is ground");
		PROP_PARAM(Color, is_ceiling, Unigine::Math::vec4_green, "Is Ceiling", "Is ceiling");
		PROP_PARAM(Color, is_crouch, Unigine::Math::vec4_blue, "Is Crouch", "Is crouch");
		PROP_PARAM(Color, average_speed, Unigine::Math::vec4(1.0f, 1.0f, 0.0f, 1.0f), "Average Speed", "Average speed"); // yellow
		PROP_PARAM(Color, auto_stepping, Unigine::Math::vec4(0.66f, 0.33f, 0.0f, 1.0f), "Auto Stepping", "Auto Stepping");

		float array_applied_horizontal_velocity[4];
		float array_applied_vertical_velocity[4];
		float array_up_pass_contacts[4];
		float array_side_pass_contacts[4];
		float array_down_pass_contacts[4];
		float array_is_ground[4];
		float array_is_ceiling[4];
		float array_is_crouch[4];
		float array_average_speed[4];
		float array_auto_stepping[4];
	};

	PROP_STRUCT(DebugColors, debug_colors, "Debug Colors", "Debug colors");

#endif

#pragma endregion

	bool isInitialized() const { return is_initialized; }

	Unigine::Math::vec3 getAdditionalCameraOffset() const { return additional_camera_offset; }
	void setAdditionalCameraOffset(const Unigine::Math::vec3 &offset) { additional_camera_offset = offset; }
	Unigine::Math::quat getAdditionalCameraRotation() const { return additional_camera_rotation; }
	void setAdditionalCameraRotation(const Unigine::Math::quat &rot) { additional_camera_rotation = rot; }

	bool isGround() const { return is_ground; }
	bool isCeiling() const { return is_ceiling; }
	bool isCrouch() const { return is_crouch; }
	bool isHorizontalFrozen() const { return is_horizontal_frozen; }

	Unigine::Math::Vec3 getSlopeNormal() const { return slope_normal; }
	Unigine::Math::Vec3 getSlopeAxisX() const { return slope_axis_x; }
	Unigine::Math::Vec3 getSlopeAxisY() const { return slope_axis_y; }

	Unigine::Math::Vec3 getHorizontalVelocity() const { return horizontal_velocity; }
	float getVerticalVelocity() const { return vertical_velocity; }

	void setWorldTransform(const Unigine::Math::Mat4 &t);

private:
	void init();
	void update();
	void update_physics();

	void update_move_directions(float ifps);
	void check_move_and_stair();
	void update_velocity(float ifps, float update_part);
	void update_collisions(float ifps);
	void try_move_up(float ifps);
	void move_side(float ifps);
	void try_move_down(float ifps);
	void update_crouch(float ifps);
	void update_camera();
	void swap_interpolation_direction(Unigine::Math::Scalar start_height, Unigine::Math::Scalar end_height);
	void update_player_height(Unigine::Math::Scalar height);

	#pragma region Debug
	#if DEBUG
	void init_debug();
	void update_debug();
	#endif
	#pragma endregion

private:
	#pragma region PlayerProperties

	bool is_initialized = false;

	Unigine::Math::vec3 additional_camera_offset = Unigine::Math::vec3_zero;
	Unigine::Math::quat additional_camera_rotation = Unigine::Math::quat_identity;

	bool is_ground = false;
	bool is_ceiling = false;
	bool is_crouch = false;
	bool is_horizontal_frozen = false;

	Unigine::Math::Vec3 slope_normal = Unigine::Math::Vec3_up;
	Unigine::Math::Vec3 slope_axis_x = Unigine::Math::Vec3_right;
	Unigine::Math::Vec3 slope_axis_y = Unigine::Math::Vec3_forward;

	Unigine::Math::Vec3 horizontal_velocity = Unigine::Math::Vec3_zero; // velocity in current slope basis
	float vertical_velocity = 0.0f;

	#pragma endregion

	// additional constants
	const Unigine::Math::vec2 forward { 0, 1 };
	const Unigine::Math::vec2 right { 1, 0 };
	const float skin_width_offset = 0.05f;
	const float auto_stepping_speed_threshold = 0.1f;
	const float large_epsilon = 0.001f;

	float player_ifps = 1.0f;

	Unigine::InputGamePadPtr game_pad;

	Unigine::BodyDummyPtr body;
	Unigine::ShapeCapsulePtr shape;
	Unigine::ShapeCylinderPtr interaction_shape;

	Unigine::Vector<Unigine::ShapeContactPtr> contacts;
	bool is_available_side_move = false;
	bool is_available_stair = false;
	bool has_bottom_contacts = false;
	bool is_heavy_contacts = false;

	float camera_vertical_angle = 0.0f;
	float camera_horizontal_angle = 0.0f;
	Unigine::Math::vec3 camera_crouch_offset = Unigine::Math::vec3_zero;

	Unigine::Math::vec2 horizontal_move_direction = Unigine::Math::vec2_zero;
	float vertical_move_direction = 0.0f;

	float max_air_speed = 0.0f;

	bool used_auto_stepping = false;
	Unigine::Math::Scalar last_step_height = 0.0f;

	float cos_ground_angle = 0.5f;
	float cos_ceiling_angle = 0.5f;
	float cos_stair_angle = 0.5f;
	float cos_stairs_detection_angle = 0.5f;
	float cos_wall_stop_sliding_angle = 0.96f;

	Unigine::Math::Mat4 world_transform = Unigine::Math::Mat4_identity;

	Unigine::WorldIntersectionNormalPtr normal_intersection;

	enum CrouchPhase
	{
		STAND = 0,
		MOVE_DOWN,
		CROUCH,
		MOVE_UP
	};

	struct CrouchState
	{
		CrouchPhase phase = CrouchPhase::STAND;
		float current_time = 0.0f;
		Unigine::Math::Scalar current_height;
		Unigine::Math::Scalar start_height;
		Unigine::Math::Scalar end_height;
	};

	CrouchState crouch_state;

	#pragma region Debug

	#if DEBUG
	float max_applied_horizontal_speed = 0.0f;
	float max_applied_vertical_speed = 20.0f;
	int max_pass_contacts = 200;
	float max_flag_value = 1.05f;

	static const int speeds_buffer_size = 10;
	float speeds_buffer[speeds_buffer_size];
	Unigine::Math::Vec3 last_player_position = Unigine::Math::Vec3_zero;

	bool auto_stepping_applied = false;
	#endif

	#pragma endregion
};