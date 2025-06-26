#pragma once
#include "Interactable.h"
#include <UnigineComponentSystem.h>

class InteractableLeverComponent
	: public Interactable
	, public Unigine::ComponentBase
{
public:
	COMPONENT(InteractableLeverComponent, ComponentBase);
	PROP_NAME("InteractableLeverComponent");

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update, Interactable::interactable_update_priority);

	PROP_PARAM(Vec3, rotation_axis, Unigine::Math::vec3(1, 0, 0));
	PROP_PARAM(Float, rotation_angle_range, 90.0f);
	PROP_PARAM(Node, transformation_node);
	PROP_PARAM(Toggle, is_movable, true);

	void use(Unigine::PlayerPtr player, Unigine::Math::vec3 intersection_point) override;
	float getValue() override;

private:
	void init();
	void update();

	Unigine::Math::vec3 lever_position;
	Unigine::Math::vec3 grab_point;
	Unigine::Math::vec3 player_vec;

	Unigine::Math::vec3 rotation_vec;
	Unigine::Math::vec3 rotation_target_vec;
	Unigine::Math::vec3 rotation_vec_old;
	Unigine::Math::vec3 valve_plane_x;
	Unigine::Math::vec3 valve_plane_y;
	Unigine::Math::vec3 valve_plane_normal;

	Unigine::Math::vec3 player_view_direction;
	Unigine::Math::vec3 player_position;

	Unigine::Math::vec3 lever_direction_off;
	Unigine::Math::quat initial_rotation;

	float rotation_angle = 0.0f;
	float rotation_angle_target = 0.0f;
	float rotation_angle_target_old = 0.0f;

	bool used_last_frame = false;
	bool used_current_frame = false;
};
