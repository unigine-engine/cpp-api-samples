// Button control element with toggle and momentary press modes.
// Animated transitions between on/off states with configurable duration.
// When togglable, each press flips state; otherwise returns to default on release.

#pragma once
#include "Interactable.h"

#include <UnigineComponentSystem.h>

// Animates between on/off positions and rotations based on button state.
class InteractableButtonComponent
	: public Interactable
	, public Unigine::ComponentBase
{

public:
	COMPONENT(InteractableButtonComponent, ComponentBase);
	PROP_NAME("InteractableButtonComponent");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, Interactable::interactable_update_priority);

	// Whether button stays in pressed state or returns on release
	PROP_PARAM(Toggle, togglable, true);
	// Initial state of the button (on/off)
	PROP_PARAM(Toggle, default_state, false);
	// Whether transition should be animated over time
	PROP_PARAM(Toggle, has_animation_time, true);
	// Duration of press/release animation in seconds
	PROP_PARAM(Float, anim_duration, 0.2f);
	// Position offset when button is in pressed state
	PROP_PARAM(Vec3, position);
	// Axis around which button rotates when pressed
	PROP_PARAM(Vec3, rotation_axis, Unigine::Math::vec3(1, 0, 0));
	// Rotation angle in degrees when button is pressed
	PROP_PARAM(Float, rotation_angle, 0.0f);
	// Node that receives position/rotation transform
	PROP_PARAM(Node, transformation_node);

	void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) override;
	float getValue() override;

private:
	// Current button state (0 = off, 1 = on)
	int button_state = false;
	void init();
	void update();

	// Rotation quaternion for pressed state
	Unigine::Math::quat rotation_on_quat;
	// Rotation quaternion for released state
	Unigine::Math::quat rotation_off_quat;

	// Position when button is released
	Unigine::Math::Vec3 position_off;
	// Position when button is pressed
	Unigine::Math::Vec3 position_on;

	// Current interpolation value (0-1) for animation
	float value = 0.0f;
	// Current animation time accumulator
	float time = 0.0f;

	// Tracks if button was pressed previous frame
	bool used_last_frame = false;
	// Tracks if button is pressed current frame
	bool used_current_frame = false;
};
