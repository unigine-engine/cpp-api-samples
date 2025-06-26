#pragma once
#include "Interactable.h"

#include <UnigineComponentSystem.h>

class InteractableButtonComponent
	: public Interactable
	, public Unigine::ComponentBase
{

public:
	COMPONENT(InteractableButtonComponent, ComponentBase);
	PROP_NAME("InteractableButtonComponent");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, Interactable::interactable_update_priority);

	PROP_PARAM(Toggle, togglable, true);
	PROP_PARAM(Toggle, default_state, false);
	PROP_PARAM(Toggle, has_animation_time, true);
	PROP_PARAM(Float, anim_duration, 0.2f);
	PROP_PARAM(Vec3, position);
	PROP_PARAM(Vec3, rotation_axis, Unigine::Math::vec3(1, 0, 0));
	PROP_PARAM(Float, rotation_angle, 0.0f);
	PROP_PARAM(Node, transformation_node);

	void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) override;
	float getValue() override;

private:
	int button_state = false;
	void init();
	void update();

	Unigine::Math::quat rotation_on_quat;
	Unigine::Math::quat rotation_off_quat;

	Unigine::Math::Vec3 position_off;
	Unigine::Math::Vec3 position_on;

	float value = 0.0f;
	float time = 0.0f;

	bool used_last_frame = false;
	bool used_current_frame = false;
};
