#pragma once

#include <UnigineComponentSystem.h>
#include "../../utils/Utils.h"


class SpectatorController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SpectatorController, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_GROUP("Input")
	PROP_PARAM(Toggle, is_controlled, 0, "Controlled", "Toggle spectator inputs");
	PROP_PARAM(String, forward_key, "W", "Forward Key", "Move forward key");
	PROP_PARAM(String, left_key, "A", "Left Key", "Move left key");
	PROP_PARAM(String, backward_key, "S", "Backward Key", "Move backward key");
	PROP_PARAM(String, right_key, "D", "Right Key", "Move right key");
	PROP_PARAM(String, down_key, "Q", "Down key", "Move down key");
	PROP_PARAM(String, up_key, "E", "Up key", "Move up key");
	PROP_PARAM(String, turn_up_key, "UP", "Turn up key", "Turn up");
	PROP_PARAM(String, turn_down_key, "DOWN", "Turn down key", "Turn down");
	PROP_PARAM(String, turn_left_key, "LEFT", "Turn left key", "Turn left");
	PROP_PARAM(String, turn_right_key, "RIGHT", "Turn right key", "Turn right");
	PROP_PARAM(String, accelerate_key, "ANY_SHIFT", "Speed-up Key", "Speed-up mode activation key");
	PROP_PARAM(Float, mouse_sensitivity, 0.4f, "Mouse Sensitivity", "Mouse sensitivity multiplier");

	PROP_GROUP("Movement")
	PROP_PARAM(Float, min_velocity, 2.0f, "Min Velocity", "Minimum movement velocity");
	PROP_PARAM(Float, max_velocity, 4.0f, "Max Velocity", "Maximum movement velocity");
	PROP_PARAM(Float, acceleration, 4.0f, "Acceleration", "Movement acceleration");
	PROP_PARAM(Float, damping, 8.0f, "Damping", "Movement damping");
	
	PROP_GROUP("Collision")
	PROP_PARAM(Toggle, is_collided, 0, "Collision", "Toggle spectator collision");
	PROP_PARAM(Mask, collision_mask, "collision", 1, "Collision Mask", "A bit mask of collisions");
	PROP_PARAM(Float, collision_radius, 0.5f, "Collision radius", "The radius of the collision sphere");

	PROP_GROUP("Rotation")
	PROP_PARAM(Float, turning, 90.0f, "Turning speed", "Velocity of the spectator's turning action");
	PROP_PARAM(Float, min_theta_angle, -89.9f, "Min theta angle", "Minimun pitch angle");
	PROP_PARAM(Float, max_theta_angle, 89.9f, "Max theta angle", "Maximum pitch angle");

	UNIGINE_INLINE float getPhiAngle() const { return phi_angle; }
	UNIGINE_INLINE float getThetaAngle() const { return theta_angle; }
	UNIGINE_INLINE const Unigine::Math::vec3 &getViewDirection() const { return direction; }

	UNIGINE_INLINE int getNumContacts() const { return contacts.size(); }
	const Unigine::ShapeContactPtr getContact(int num) const;
	float getContactDepth(int num) const;
	Unigine::Math::vec3 getContactNormal(int num) const;
	Unigine::ObjectPtr getContactObject(int num);
	Unigine::Math::Vec3 getContactPoint(int num) const;
	Unigine::ShapePtr getContactShape(int num) const;
	int getContactSurface(int num) const;

	void setPhiAngle(float newAngle);
	void setThetaAngle(float newAngle);
	void setViewDirection(const Unigine::Math::vec3& newView);

	void updateControls();
	void flushTransform();

private:
	void init();
	void update();
	void shutdown();

	void on_transform_changed();

	void update_movement(const Unigine::Math::vec3& impulse, float ifps, float target_velocity);

private:

	Unigine::Math::Vec3 position = Unigine::Math::Vec3_zero;
	Unigine::Math::vec3 direction = Unigine::Math::vec3_zero;

	Unigine::Math::Mat4 transform = Unigine::Math::Mat4_zero;

	float phi_angle = 0.0f;
	float theta_angle = 0.0f;

	Unigine::Math::Vec3 last_position = Unigine::Math::Vec3_zero;
	Unigine::Math::vec3 last_direction = Unigine::Math::vec3_zero;
	Unigine::Math::vec3 last_up = Unigine::Math::vec3_zero;

	Unigine::Vector<Unigine::ShapeContactPtr> contacts{nullptr};

	Unigine::Input::KEY _forward_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _backward_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _right_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _left_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _up_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _down_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _turn_up_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _turn_down_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _turn_left_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _turn_right_key = Unigine::Input::KEY_UNKNOWN;
	Unigine::Input::KEY _accelerate_key = Unigine::Input::KEY_UNKNOWN;

	Unigine::ShapeSpherePtr p_shape_sphere = nullptr;
	Unigine::PlayerPtr p_player = nullptr;

	const float PLAYER_SPECTATOR_IFPS = 1.0f / 60.0f;
	const int PLAYER_SPECTATOR_COLLISIONS = 4;
};