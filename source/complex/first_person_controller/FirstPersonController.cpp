#include "FirstPersonController.h"
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <UnigineProfiler.h>
#include <UnigineConsole.h>
#include <UnigineInput.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(FirstPersonController)

void FirstPersonController::setWorldTransform(const Unigine::Math::Mat4 &t)
{
	// fix player transformation
	// player can only have vertical position, Y axis is used for forward direction
	Vec3 axis_y = t.getAxisY();
	axis_y.z = 0;
	axis_y = (axis_y.length2() > Consts::EPS ? axis_y.normalize() : Vec3_forward);
	node->setWorldTransform(setTo(t.getTranslate(), t.getTranslate() + axis_y, vec3_up, AXIS_Y));

	if (camera && camera_mode != CameraMode::NONE)
	{
		camera_vertical_angle = getAngle(vec3_down, static_cast<vec3>(-t.getAxisZ()));
		camera_vertical_angle = clamp(camera_vertical_angle, min_vertical_angle + 90.0f, max_vertical_angle + 90.0f);

		camera_horizontal_angle = node->getWorldRotation().getAngle(vec3_up);

		vec3 camera_direction = vec3_forward * rotateZ(-camera_horizontal_angle);
		camera_direction = camera_direction * rotate(cross(camera_direction, vec3_up), 90.0f - camera_vertical_angle);
		camera_direction.normalize();
		camera->setWorldDirection(camera_direction, vec3_up);
	}
}

void FirstPersonController::init()
{
	// check object type
	ObjectPtr obj = checked_ptr_cast<Object>(node);
	if (!obj)
	{
		Log::error("FirstPersonController: can't cast node to Object\n");
		return;
	}

	// fix player transformation
	// player can only have vertical position, Y axis is used for forward direction
	Vec3 axis_y = obj->getWorldTransform().getAxisY();
	axis_y.z = 0;
	axis_y = (axis_y.length2() > Consts::EPS ? axis_y.normalize() : Vec3_forward);
	obj->setWorldTransform(setTo(node->getWorldPosition(), node->getWorldPosition() + axis_y, vec3_up, AXIS_Y));

	// set dummy body
	if (use_object_body)
	{
		body = checked_ptr_cast<BodyDummy>(obj->getBody());
		if (!body)
			Log::warning("FirstPersonController: object doesn't contain BodyDummy, it was created automatically\n");
	}

	if (!body)
	{
		body = BodyDummy::create(obj);
		body->setTransform(obj->getWorldTransform());
	}

	// set capsule shape
	if (use_object_body)
	{
		if (body->getNumShapes() > 0)
		{
			for (int i = 0; i < body->getNumShapes(); i++)
				if (!shape)
					shape = checked_ptr_cast<ShapeCapsule>(body->getShape(i));

			if (!shape)
				Log::warning("FirstPersonController: body doesn't contain ShapeCapsule, it was created automatically\n");
		}
		else
			Log::warning("FirstPersonController: body doesn't contain shapes, it was created automatically\n");
	}

	if (!shape)
	{
		shape = ShapeCapsule::create(body, capsule_radius, capsule_height);
		body->setShapeTransform(body->getNumShapes() - 1, translate(vec3_up * (capsule_radius + 0.5f * capsule_height)));

		shape->setPhysicsIntersectionMask(physics_intersection_mask);
		shape->setCollisionMask(collision_mask);
		shape->setExclusionMask(exclusion_mask);
	}

	capsule_height = shape->getHeight();
	crouch_height = clamp(crouch_height, 2.0f * shape->getRadius(), shape->getHeight() + 2.0f * shape->getRadius());
	crouch_state.current_height = shape->getHeight() + 2.0f * shape->getRadius();
	crouch_state.start_height = crouch_state.current_height;
	crouch_state.end_height = crouch_height.get();

	// set camera
	camera = checked_ptr_cast<PlayerDummy>(camera_editor_node.get());

	if (camera_mode == CameraMode::USE_EXTERNAL && !camera)
		Log::warning("FirstPersonController: camera is null, it was created automatically\n");

	if (!camera || camera_mode == CameraMode::CREATE_AUTOMATICALLY)
	{
		camera = PlayerDummy::create();
		camera->setParent(obj);
		camera->setFov(fov);
		camera->setZNear(near_clipping);
		camera->setZFar(far_clipping);

		camera->setWorldPosition(obj->getWorldTransform() * Vec3(camera_position_offset));
		camera->setWorldDirection(vec3(axis_y), vec3_up);

		camera->setMainPlayer(true);
	}

	if (camera && camera_mode != CameraMode::NONE)
	{
		camera_vertical_angle = getAngle(vec3_down, camera->getWorldDirection());
		camera_vertical_angle = clamp(camera_vertical_angle, min_vertical_angle + 90.0f, max_vertical_angle + 90.0f);

		camera_horizontal_angle = node->getWorldRotation().getAngle(vec3_up);
		camera_position_offset = vec3(node->getIWorldTransform() * camera->getWorldPosition());

		vec3 camera_direction = vec3_forward * rotateZ(-camera_horizontal_angle);
		camera_direction = camera_direction * rotate(cross(camera_direction, vec3_up), 90.0f - camera_vertical_angle);
		camera_direction.normalize();
		camera->setWorldDirection(camera_direction, vec3_up);
	}

	// create cylinder shape for interacting with objects
	if (use_objects_interaction)
	{
		interaction_shape = ShapeCylinder::create();
		interaction_shape->setEnabled(false);
	}

	// set auxiliary parameters
	player_ifps = 1.0f / player_fps;
	crouch_transition_time = max(crouch_transition_time, Consts::EPS);

	cos_ground_angle = Math::cos(max_ground_angle * Math::Consts::DEG2RAD);
	cos_ceiling_angle = Math::cos(max_ceiling_angle * Math::Consts::DEG2RAD);
	cos_stair_angle = Math::cos(max_stair_angle * Math::Consts::DEG2RAD);
	cos_stairs_detection_angle = Math::cos(stairs_detection_angle * Math::Consts::DEG2RAD + Math::Consts::PI05);
	cos_wall_stop_sliding_angle = Math::cos(wall_stop_sliding_angle * Math::Consts::DEG2RAD);

	normal_intersection = WorldIntersectionNormal::create();

#if DEBUG
	max_pass_contacts = contacts_buffer_size;
#endif

	world_transform = obj->getWorldTransform();

	for (int i = 0; i < Input::getNumGamePads(); i++)
	{
		if (Input::getGamePad(i)->isAvailable())
		{
			game_pad = Input::getGamePad(i);
			break;
		}
	}

	is_initialized = true;

#if DEBUG
	init_debug();
#endif
}

void FirstPersonController::update()
{
	if (!is_initialized)
		return;

	world_transform = node->getWorldTransform();

	float ifps = Game::getIFps() * Physics::getScale();

	update_move_directions(ifps);
	check_move_and_stair();

	if (!is_available_side_move)
	{
		if (has_bottom_contacts)
		{
			horizontal_velocity = Vec3_zero;
			horizontal_move_direction = vec2_zero;
		}
	}

	float update_time = ifps;
	has_bottom_contacts = false;
	while (update_time > 0.0f)
	{
		float adaptive_time_step = min(update_time, player_ifps);
		update_time -= adaptive_time_step;

		update_velocity(adaptive_time_step, adaptive_time_step / ifps);
		mul(world_transform, translate((horizontal_velocity + Vec3_up * vertical_velocity) * adaptive_time_step), world_transform);
		update_collisions(adaptive_time_step);
	}

	update_crouch(ifps);

	// update player transformation
	node->setWorldTransform(world_transform);
	body->setTransform(node->getWorldTransform());
	if (is_crouch)
		shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

	update_camera();

#if DEBUG
	update_debug();
#endif
}

void FirstPersonController::update_physics()
{
	if (!is_initialized)
		return;

	if (use_objects_interaction)
	{
		// enable interaction cylinder shape and set parameters
		// this shape is slightly larger than the capsule shape
		// this allows to get more correct contacts with objects, especially with bottom of player
		interaction_shape->setEnabled(true);
		interaction_shape->setRadius(shape->getRadius() + check_move_ray_offset + skin_width_offset);
		interaction_shape->setHeight(shape->getHeight() + 2.0f * (shape->getRadius() + check_move_ray_offset - skin_width_offset));
		interaction_shape->setTransform(translate(world_transform.getTranslate() + Vec3_up * (0.5f * interaction_shape->getHeight() + skin_width_offset)));

		interaction_shape->getCollision(contacts);
		int contacts_count = min(contacts_buffer_size, contacts.size());

		Scalar speed = horizontal_velocity.length() + Math::abs(vertical_velocity);
		speed = max(speed, static_cast<Scalar>(1.0f));

		// multiply by 0.5f only for normalization impulse multiplier in editor settings
		float k = player_mass * impulse_multiplier * 0.5f;
		if (contacts_count > 0)
			k /= contacts_count;

		float top_cap_z = shape->getTopCap().z;
		for (int i = 0; i < contacts_count; i++)
		{
			const ShapeContactPtr &c = contacts[i];
			if (auto contact_object = c->getObject())
			{
				if (auto rb = contact_object->getBodyRigid())
				{
					Vec3 point = c->getPoint();
					if (ignore_top_contacts && point.z > top_cap_z)
						continue;

					rb->setFrozen(false);
					rb->addWorldImpulse(point, -c->getNormal() * k * speed);
				}
			}
		}

		interaction_shape->setEnabled(false);
	}
}

void FirstPersonController::update_move_directions(float ifps)
{
	// reset all directions
	horizontal_move_direction = vec2_zero;
	vertical_move_direction = 0.0f;

	if (!Input::isMouseGrab())
		return;

	// update horizontal direction
	if (Input::isKeyPressed(static_cast<Input::KEY>(forward_key.get())))
		horizontal_move_direction += forward;

	if (Input::isKeyPressed(static_cast<Input::KEY>(backward_key.get())))
		horizontal_move_direction -= forward;

	if (Input::isKeyPressed(static_cast<Input::KEY>(right_key.get())))
		horizontal_move_direction += right;

	if (Input::isKeyPressed(static_cast<Input::KEY>(left_key.get())))
		horizontal_move_direction -= right;

	horizontal_move_direction.normalize();

	if (game_pad)
	{
		vec2 moveValue = (move_stick == GamepadStickSide::LEFT ? game_pad->getAxesLeft() : game_pad->getAxesRight());
		if (moveValue.length() > sticks_dead_zone && moveValue.length2() > horizontal_move_direction.length2())
			horizontal_move_direction = moveValue;
	}

	// update vertical direction
	if (use_jump && is_ground && (Input::isKeyDown(static_cast<Input::KEY>(jump_key.get())) ||
		game_pad && game_pad->isButtonDown(static_cast<Input::GAMEPAD_BUTTON>(jump_button.get()))))
	{
		vertical_move_direction = (is_crouch ? crouch_jump_power : jump_power) / ifps;
	}
}

void FirstPersonController::check_move_and_stair()
{
	is_available_side_move = false;
	is_available_stair = false;

	vec3 horizontal_direction = world_transform.getRotate() * vec3(horizontal_move_direction);
	horizontal_move_direction.normalize();

	// check angle of surface for possible movement
	if (horizontal_move_direction.length2() > 0.0f)
	{
		Vec3 p2 = world_transform.getTranslate() + Vec3(horizontal_direction) * (static_cast<Scalar>(shape->getRadius()) + check_move_ray_offset) + Vec3_down * Scalar(check_move_ray_offset.get());
		Vec3 p1 = p2 + Vec3_up * (max(shape->getRadius(), max_step_height) + check_move_ray_offset);

		auto hitObj = World::getIntersection(p1, p2, check_move_mask, normal_intersection);
		if (hitObj)
		{
			if (dot(vec3_up, normal_intersection->getNormal()) > cos_ground_angle)
				is_available_side_move = true;

			// this check allows movement through elevations
			Scalar cos = dot(slope_normal, Vec3(normal_intersection->getNormal()));
			if (cos < large_epsilon)
				is_available_side_move = true;
		}
		else
		{
			// allow to move in air
			is_available_side_move = true;
		}

	#if DEBUG
		if (debug_visualizer.get().enabled && debug_visualizer.get().check_move_ray)
		{
			if (is_available_side_move)
				Visualizer::renderVector(p1, p2, vec4_green);
			else
				Visualizer::renderVector(p1, p2, vec4_red);
		}
	#endif
	}

	// check stair surface angle for auto stepping
	if (use_auto_stepping && horizontal_move_direction.length2() > 0.0f)
	{
		Vec3 p2 = world_transform.getTranslate() + Vec3(horizontal_direction)
			* static_cast<Scalar>(shape->getRadius() + check_stair_ray_offset)
			+ Vec3_up * static_cast<Scalar>(min_step_height);
		Vec3 p1 = p2 + Vec3(vec3_up)
			* static_cast<Scalar>(max_step_height.get() - min_step_height.get() + check_stair_ray_offset.get());

		auto hitObj = World::getIntersection(p1, p2, stair_detection_mask, normal_intersection);
		if (hitObj)
		{
			if (dot(vec3_up, normal_intersection->getNormal()) > cos_stair_angle)
				is_available_stair = true;
		}

	#if DEBUG
		if (debug_visualizer.get().enabled && debug_visualizer.get().stair_detection_ray)
		{
			if (is_available_stair)
				Visualizer::renderVector(p1, p2, vec4_green);
			else
				Visualizer::renderVector(p1, p2, vec4_red);
		}
	#endif
	}
}

void FirstPersonController::update_velocity(float ifps, float update_part)
{
	// update current slope basis
	// check vectors for collinearity and, depending on this, calculate the slope basis
	Scalar cos_angle = dot(Vec3(world_transform.getAxisY()), slope_normal);
	if (compare(Math::abs(cos_angle), static_cast<Scalar>(1.0f)) == 0)
	{
		slope_axis_x = cross(Vec3(world_transform.getAxisY()), slope_normal).normalize();
		slope_axis_y = cross(slope_normal, slope_axis_x).normalize();
	}
	else
	{
		slope_axis_y = cross(Vec3(world_transform.getAxisX()) * sign(cos_angle), slope_normal).normalize();
		slope_axis_x = cross(slope_axis_y, slope_normal).normalize();
	}

	// get decomposition of velocity for instant change on ground
	Vec3 horizontal_velocity_decomposition = Vec3_zero;
	if (is_ground)
	{
		horizontal_velocity_decomposition.x = dot(slope_axis_x, horizontal_velocity);
		horizontal_velocity_decomposition.y = dot(slope_axis_y, horizontal_velocity);
		horizontal_velocity_decomposition.z = dot(slope_normal, horizontal_velocity);
	}

	// player rotation
	if (Input::isMouseGrab())
	{
		world_transform *= Mat4(rotate(quat(vec3_up, -Input::getMouseDeltaPosition().x * mouse_sensitivity * update_part)));

		float delta = -Input::getMouseDeltaPosition().x * mouse_sensitivity;
		if (game_pad)
		{
			vec2 rotate_value = (camera_stick == GamepadStickSide::LEFT ? game_pad->getAxesLeft() : game_pad->getAxesRight());
			if (rotate_value.length() > sticks_dead_zone && Math::abs(rotate_value.x * camera_stick_sensitivity) > Math::abs(delta))
				delta = -rotate_value.x * camera_stick_sensitivity;
		}

		camera_horizontal_angle += delta * update_part;
		if (camera_horizontal_angle < -180.0f || 180.0f < camera_horizontal_angle)
			camera_horizontal_angle -= sign(camera_horizontal_angle) * 360.0f;

		Vec3 position = world_transform.getTranslate();
		world_transform.setRotate(Vec3_up, camera_horizontal_angle);
		world_transform.setColumn3(3, position);
	}

	// on the ground change velocity without inertia
	if (is_ground)
	{
		// again check vectors for collinearity and, depending on this, update the slope basis
		cos_angle = dot(Vec3(world_transform.getAxisY()), slope_normal);
		if (compare(Math::abs(cos_angle), static_cast<Scalar>(1.0f)) == 0)
		{
			slope_axis_x = cross(Vec3(world_transform.getAxisY()), slope_normal).normalize();
			slope_axis_y = cross(slope_normal, slope_axis_x).normalize();
		}
		else
		{
			slope_axis_y = cross(Vec3(world_transform.getAxisX()) * sign(cos_angle), slope_normal).normalize();
			slope_axis_x = cross(slope_axis_y, slope_normal).normalize();
		}

		// restore velocity in new basis
		horizontal_velocity = slope_axis_x * horizontal_velocity_decomposition.x +
			slope_axis_y * horizontal_velocity_decomposition.y +
			slope_normal * horizontal_velocity_decomposition.z;
	}

	// add horizontal velocity in slope basis
	float acceleration = (is_ground ? ground_acceleration : air_acceleration);
	horizontal_velocity += slope_axis_x * horizontal_move_direction.x * acceleration * ifps;
	horizontal_velocity += slope_axis_y * horizontal_move_direction.y * acceleration * ifps;

	// update vertical velocity
	vertical_velocity += vertical_move_direction * ifps;
	if (!is_ground)
		vertical_velocity += Physics::getGravity().z * ifps;

	// get current max speed
	float max_speed = max_air_speed;
	if (is_ground)
	{
		max_speed = (use_run && use_run_default) ? run_speed : walk_speed;

		if (use_run && (Input::isKeyPressed(static_cast<Input::KEY>(run_key.get())) ||
			game_pad && game_pad->isButtonPressed(static_cast<Input::GAMEPAD_BUTTON>(run_button.get()))))
		{
			max_speed = use_run_default ? walk_speed : run_speed;
		}

		if (is_ground && is_crouch)
			max_speed = crouch_speed;

		max_air_speed = max_speed;
	}

	// apply damping to horizontal velocity when it exceeds target speed
	// or target speed too low (not pressed horizontal movement keys)
	vec2 targetSpeed = horizontal_move_direction * max_speed;
	if (targetSpeed.length() < Consts::EPS || targetSpeed.length() < horizontal_velocity.length())
		horizontal_velocity *= Math::exp((is_ground ? -ground_damping : -air_damping) * ifps);

	// clamp horizontal velocity if it greater than current max speed
	if (horizontal_velocity.length() > max_speed)
		horizontal_velocity = horizontal_velocity.normalize() * max_speed;

	// check frozen state for horizontal velocity
	// IsGround needed in case of slipping from the edges
	// contacts will be pushed player in all directions, and not just up
	is_horizontal_frozen = is_ground && (horizontal_velocity.length() < Physics::getFrozenLinearVelocity());
}

void FirstPersonController::update_collisions(float ifps)
{
	// set default collision parameters
	is_ground = false;
	is_ceiling = false;
	slope_normal = Vec3_up;

	is_heavy_contacts = false;

	// resolve current collisions
	for (int j = 0; j < collision_iterations; j++)
	{
		if (use_auto_stepping)
		{
		#if DEBUG
			auto_stepping_applied = false;
		#endif
			if (is_available_stair)
				try_move_up(ifps);
		}

		move_side(ifps);

		if (use_auto_stepping && used_auto_stepping && is_available_stair)
		{
		#if DEBUG
			auto_stepping_applied = true;
		#endif
			try_move_down(ifps);
		}

		if (is_heavy_contacts)
			break;
	}
}

void FirstPersonController::try_move_up(float ifps)
{
	used_auto_stepping = false;
	last_step_height = 0.0f;

	if (horizontal_move_direction.length2() > 0.0f && !is_horizontal_frozen && vertical_velocity < 0.0f)
	{
		body->setTransform(world_transform);
		if (is_crouch)
			shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

		// find collisions with the capsule
		shape->getCollision(contacts);
		if (contacts.size() == 0)
			return;

		if (contacts.size() > heavy_contacts_count)
			is_heavy_contacts = true;

		int contacts_count = min(contacts_buffer_size, contacts.size());

		// find max step height
		Vec2 velocity_xy = Vec2(horizontal_velocity);
		if (velocity_xy.length2() < auto_stepping_speed_threshold)
		{
			// set minimal velocity for climb
			velocity_xy = Vec2(world_transform.getRotate() * Vec3(Vec2(horizontal_move_direction)));
			velocity_xy.normalize();
			horizontal_velocity = Vec3(velocity_xy * static_cast<Scalar>(walk_speed));
		}

		for (int i = 0; i < contacts_count; i++)
		{
			const ShapeContactPtr &c = contacts[i];

			Vec2 normalXY = Vec2(Vec3(c->getNormal()));

			// skip contacts opposite to movement
			if (dot(normalXY, velocity_xy) > cos_stairs_detection_angle)
				continue;

			Scalar step = dot(c->getPoint() - world_transform.getTranslate(), Vec3_up);
			if (last_step_height < step)
				last_step_height = step;
		}

		// apply auto stepping
		if (min_step_height < last_step_height && last_step_height < max_step_height)
		{
			world_transform.setColumn3(3, world_transform.getTranslate() + Vec3(vec3_up) * last_step_height);

			// check contacts with other objects after elevating
			// and cancel automatic step if contacts exist
			body->setTransform(world_transform);
			if (is_crouch)
				shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

			shape->getCollision(contacts);
			if (contacts.size() == 0)
				used_auto_stepping = true;
			else
				world_transform.setColumn3(3, world_transform.getTranslate() + Vec3(vec3_down) * last_step_height);
		}

	#if DEBUG
		if (debug_visualizer.get().enabled && debug_visualizer.get().up_pass_contacts)
		{
			for (const auto &c : contacts)
				Visualizer::renderVector(c->getPoint(), c->getPoint() + Vec3(c->getNormal()), debug_colors.get().up_pass_contacts);
		}

		if (debug_profiler.get().enabled && debug_profiler.get().up_pass_contact)
			Profiler::setValue("Up Pass Contacts ", "", contacts.size(), max_pass_contacts, debug_colors.get().array_up_pass_contacts);
	#endif
	}
}

void FirstPersonController::move_side(float ifps)
{
	// apply new player transformation for physic body
	body->setTransform(world_transform);
	if (is_crouch)
		shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

	// get contacts in new position and process them
	shape->getCollision(contacts);
	if (contacts.size() == 0)
		return;

	if (contacts.size() > heavy_contacts_count)
		is_heavy_contacts = true;

	int contacts_count = min(contacts_buffer_size, contacts.size());

	// total position offset for all contacts depth
	auto position_offset = vec3_zero;

	// maximum angle of inclination of the surface under the player
	float max_cos_angle = 1.0f;

	float inum = 1.0f / contacts_count;
	for (int i = 0; i < contacts_count; i++)
	{
		const ShapeContactPtr &c = contacts[i];

		// when horizontal velocity is frozen, we can move player only in vertical direction
		// this help to avoid sliding on slopes
		// in other cases, move player in all directions
		// use epsilon offset with depth for accuracy ground detection
		if (is_horizontal_frozen)
		{
			float depth = dot(vec3_up, c->getNormal()) * (c->getDepth() - Consts::EPS);
			position_offset += vec3_up * depth * inum;
		}
		else
		{
			position_offset += c->getNormal() * (c->getDepth() - Consts::EPS) * inum;

			// remove part of horizontal velocity that is projected onto normal of current contact
			Scalar normal_speed = dot(Vec3(c->getNormal()), horizontal_velocity);
			horizontal_velocity -= Vec3(c->getNormal()) * normal_speed;
		}

		// stop sliding near the wall at a certain angle
		if ((c->getObject() && !c->getObject()->getBodyRigid()) && shape->getBottomCap().z < c->getPoint().z && c->getPoint().z < shape->getTopCap().z)
		{
			float cos = dot(world_transform.getRotate() * vec3(horizontal_move_direction), -c->getNormal());
			if (cos > cos_wall_stop_sliding_angle)
				horizontal_velocity = Vec3_zero;
		}

		// check ground state
		// first part of expression checks that current contact belongs to bottom sphere of capsule
		// second part of expression checks that current angle of inclination of surface
		// not exceed maximum allowed angle
		if (dot(c->getPoint() - shape->getBottomCap(), Vec3_up) < 0.0f)
		{
			has_bottom_contacts = true;
			if (dot(c->getNormal(), vec3_up) > cos_ground_angle)
			{
				vertical_velocity = Physics::getGravity().z * ifps;
				is_ground = true;
			}

			// find to maximum angle of inclination of surface under player
			// and save normal of this surface
			float cos_angle = dot(vec3_up, c->getNormal());
			if (compare(cos_angle, 0.0f, 0.01f) == 0 && cos_angle < max_cos_angle)
			{
				slope_normal = Vec3(contacts[i]->getNormal());
				max_cos_angle = cos_angle;
			}
		}

		// check ceiling state
		// first part of expression checks that current angle of inclination of ceiling
		// not exceed maximum allowed angle
		// second part of expression checks that current contact belongs to top sphere of capsule
		if (dot(contacts[i]->getNormal(), vec3_down) > cos_ceiling_angle && dot(contacts[i]->getPoint() - shape->getTopCap(), Vec3_down) < 0.0f)
		{
			is_ceiling = true;

			// stop moving up
			vertical_velocity = 0.0f;
		}
	}

	// add total position offset to player transformation
	world_transform.setColumn3(3, world_transform.getTranslate() + Vec3(position_offset));

#if DEBUG
	if (debug_visualizer.get().enabled && debug_visualizer.get().side_pass_contacts)
	{
		for (const auto &c : contacts)
			Visualizer::renderVector(c->getPoint(), c->getPoint() + Vec3(c->getNormal()), debug_colors.get().side_pass_contacts);
	}

	if (debug_profiler.get().enabled && debug_profiler.get().side_pass_contact)
		Profiler::setValue("Side Pass Contacts ", "", contacts.size(), max_pass_contacts, debug_colors.get().array_side_pass_contacts);
#endif
}

void FirstPersonController::try_move_down(float ifps)
{
	// this correction allows to avoid jittering on large stairs
	if (last_step_height > shape->getRadius())
		last_step_height = shape->getRadius() - Physics::getPenetrationTolerance();

	// try to drop down the player
	world_transform.setColumn3(3, world_transform.getTranslate() - Vec3(vec3_up) * last_step_height);

	body->setTransform(world_transform);
	if (is_crouch)
		shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

	// find collisions with the capsule
	shape->getCollision(contacts);
	if (contacts.size() == 0)
		return;

	if (contacts.size() > heavy_contacts_count)
		is_heavy_contacts = true;

	int contacts_count = min(contacts_buffer_size, contacts.size());

	float inum_contacts = 1.0f / itof(contacts_count);
	for (int i = 0; i < contacts_count; i++)
	{
		const ShapeContactPtr &c = contacts[i];

		float depth = dot(vec3_up, c->getNormal()) * c->getDepth();
		world_transform.setColumn3(3, world_transform.getTranslate()
			+ Vec3(vec3_up) * static_cast<Scalar>(depth) * static_cast<Scalar>(inum_contacts));

		if (dot(c->getNormal(), vec3_up) > cos_ground_angle && dot(c->getPoint() - shape->getBottomCap(), Vec3_up) < 0.0f)
		{
			is_ground = true;
			vertical_velocity = Physics::getGravity().z * ifps;
		}
	}

#if DEBUG
	if (debug_visualizer.get().enabled && debug_visualizer.get().down_pass_contacts)
	{
		for(const auto &c : contacts)
			Visualizer::renderVector(c->getPoint(), c->getPoint() + Vec3(c->getNormal()), debug_colors.get().down_pass_contacts);
	}

	if (debug_profiler.get().enabled && debug_profiler.get().down_pass_contact)
		Profiler::setValue("Down Pass Contacts ", "", contacts.size(), max_pass_contacts, debug_colors.get().array_down_pass_contacts);
#endif
}

void FirstPersonController::update_crouch(float ifps)
{
	if (!use_crouch)
		return;

	// get state of crouch key
	bool is_key = (Input::isKeyPressed(static_cast<Input::KEY>(crouch_key.get())) ||
				  game_pad && game_pad->isButtonPressed(static_cast<Input::GAMEPAD_BUTTON>(crouch_button.get())));

	// determine the subsequent behavior depending on the current phase
	switch (crouch_state.phase)
	{
		case CrouchPhase::STAND:
			if (is_key)
			{
				// go into a state of smooth movement down
				// set begin height to full player height
				// set end height to crouch player height
				// and activate crouch state
				crouch_state.phase = CrouchPhase::MOVE_DOWN;
				swap_interpolation_direction(capsule_height + 2.0f * shape->getRadius(), crouch_height);
				is_crouch = true;
			}
			break;

		case CrouchPhase::MOVE_DOWN:
		case CrouchPhase::CROUCH:
			if (!is_key)
			{
				// set player’s full height and check if we can get up
				bool can_stand_up = true;

				// set shape parameters for standing position
				// use width offset to avoid false wall contacts
				float radius = shape->getRadius();
				shape->setRadius(radius - skin_width_offset);
				update_player_height(capsule_height + 2.0f * skin_width_offset);

				// check current collisions
				shape->getCollision(contacts);

				Scalar top_point = world_transform.getTranslate().z + crouch_height;
				for (int i = 0; i < contacts.size(); i++)
					if (contacts[i]->getPoint().z > top_point)
					{
						// some collisions are higher than crouch height and we can't stand up
						can_stand_up = false;
						break;
					}

				// set current shape parameters
				shape->setRadius(radius);
				update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());

				if (can_stand_up)
				{
					// go into a state of smooth movement up
					// set begin height to crouch player height
					// set end height to full player height
					crouch_state.phase = CrouchPhase::MOVE_UP;
					swap_interpolation_direction(crouch_height, capsule_height + 2.0f * shape->getRadius());
				}
			}
			break;

		case CrouchPhase::MOVE_UP:
			if (is_ceiling || is_key)
			{
				// if we touched an obstacle from above or the key is pressed again,
				// we go into a state of smooth movement down
				// set begin height to full player height
				// set end height to crouch player height
				crouch_state.phase = CrouchPhase::MOVE_DOWN;
				swap_interpolation_direction(capsule_height + 2.0f * shape->getRadius(), crouch_height);
			}
			break;

		default: break;
	}

	// handle smooth motion
	if (crouch_state.current_time > 0.0f)
	{
		// get current linear interpolation coefficient based on current phase time
		float t = 1.0f;
		if (compare(crouch_transition_time, Consts::EPS) == 0)
			t = saturate(1.0f - crouch_state.current_time / crouch_transition_time);

		// update current player height
		crouch_state.current_height = lerp(crouch_state.start_height, crouch_state.end_height, static_cast<Scalar>(t));
		update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());

		crouch_state.current_time -= ifps;

		// handle final step of smooth motion
		if (crouch_state.current_time <= 0.0f)
		{
			// set final time and height
			crouch_state.current_time = 0.0f;
			crouch_state.current_height = crouch_state.end_height;

			switch (crouch_state.phase)
			{
				case CrouchPhase::MOVE_DOWN:
					// set crouch player height and go into crouch phase
					update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());
					crouch_state.phase = CrouchPhase::CROUCH;
					break;

				case CrouchPhase::MOVE_UP:
					// set full player height and go into stand phase
					// also disable crouch state
					update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());
					crouch_state.phase = CrouchPhase::STAND;
					is_crouch = false;
					break;

				default: break;
			}
		}
	}
}

void FirstPersonController::update_camera()
{
	if (!camera || camera_mode == CameraMode::NONE)
		return;

	if (Input::isMouseGrab())
	{
		// change vertical angle of camera
		camera_vertical_angle -= Input::getMouseDeltaPosition().y * mouse_sensitivity;

		float delta = -Input::getMouseDeltaPosition().y * mouse_sensitivity;
		if (game_pad)
		{
			vec2 rotate_value = (camera_stick == GamepadStickSide::LEFT ? game_pad->getAxesLeft() : game_pad->getAxesRight());
			if (rotate_value.length() > sticks_dead_zone && Math::abs(rotate_value.y * camera_stick_sensitivity) > Math::abs(delta))
				delta = rotate_value.y * camera_stick_sensitivity;
		}

		camera_vertical_angle += delta;
		camera_vertical_angle = clamp(camera_vertical_angle, min_vertical_angle + 90.0f, max_vertical_angle + 90.0f);
	}

	// update camera transformation taking into account all additional offsets of position and rotation
	camera->setWorldPosition(world_transform * (Vec3(camera_position_offset.get()) + Vec3(camera_crouch_offset) + Vec3(additional_camera_offset)));

	vec3 camera_direction = vec3_forward * rotateZ(-camera_horizontal_angle);
	camera_direction = camera_direction * rotate(cross(camera_direction, vec3_up), 90.0f - camera_vertical_angle);
	camera_direction = additional_camera_rotation * camera_direction;
	camera_direction.normalize();
	camera->setWorldDirection(camera_direction, vec3_up);
}

void FirstPersonController::swap_interpolation_direction(Scalar start_height, Scalar end_height)
{
	crouch_state.current_time = max(Consts::EPS, crouch_transition_time - crouch_state.current_time);
	crouch_state.start_height = start_height;
	crouch_state.end_height = end_height;
}

void FirstPersonController::update_player_height(Scalar height)
{
	shape->setHeight((float)height);
	camera_crouch_offset = vec3_up * (height - capsule_height);
	shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + height * 0.5f)));
}

#if DEBUG

void FirstPersonController::init_debug()
{
	if (!is_initialized)
		return;

	// debug camera
	if (debug_camera.get().enabled)
	{
		debug_camera.get().camera = PlayerDummy::create();
		debug_camera.get().angular_speed = 90.0f;
		debug_camera.get().zoom_speed = 3.0f;
		debug_camera.get().max_distance = 10.0f;
		debug_camera.get().horizontal_angle = 0.0f;
		debug_camera.get().vertical_angle = 0.0f;
		debug_camera.get().distance = debug_camera.get().max_distance * 0.5f;

		Game::setPlayer(debug_camera.get().camera);

		debug_camera.get().camera->setWorldDirection(vec3_forward, vec3_up);
		debug_camera.get().camera->setWorldPosition((world_transform.getTranslate() + Vec3(vec3_up)
			* static_cast<Scalar>(shape->getRadius() + shape->getHeight() * 0.5f)) - Vec3(vec3_forward)
			* static_cast<Scalar>(debug_camera.get().distance));
	}

	// use visualizer
	String command;

	Visualizer::setEnabled(debug_visualizer.get().enabled != 0);
	if (debug_visualizer.get().enabled)
	{
		Render::setShowTriangles(debug_visualizer.get().triangles ? 1 : 0);

		int showShapes = (debug_visualizer.get().shapes ? 1 : 0);
		command = "physics_show_shapes " + String::itoa(showShapes);
		Console::run(command);
	}

	// use profiler
	int show_profiler = (debug_profiler.get().enabled ? 1 : 0);
	command = "show_profiler " + String::itoa(show_profiler);
	Console::run(command);
	if (debug_profiler.get().enabled)
	{
		// applied horizontal speed
		max_applied_horizontal_speed = max(crouch_speed, walk_speed);
		max_applied_horizontal_speed = max(max_applied_horizontal_speed, run_speed);
		max_applied_horizontal_speed *= 1.1f;

		debug_colors.get().array_applied_horizontal_velocity[0] = debug_colors.get().applied_horizontal_velocity.get().x;
		debug_colors.get().array_applied_horizontal_velocity[1] = debug_colors.get().applied_horizontal_velocity.get().y;
		debug_colors.get().array_applied_horizontal_velocity[2] = debug_colors.get().applied_horizontal_velocity.get().z;
		debug_colors.get().array_applied_horizontal_velocity[3] = debug_colors.get().applied_horizontal_velocity.get().w;

		// applied vertical speed
		debug_colors.get().array_applied_vertical_velocity[0] = debug_colors.get().applied_vertical_velocity.get().x;
		debug_colors.get().array_applied_vertical_velocity[1] = debug_colors.get().applied_vertical_velocity.get().y;
		debug_colors.get().array_applied_vertical_velocity[2] = debug_colors.get().applied_vertical_velocity.get().z;
		debug_colors.get().array_applied_vertical_velocity[3] = debug_colors.get().applied_vertical_velocity.get().w;

		// up pass contacts
		debug_colors.get().array_up_pass_contacts[0] = debug_colors.get().up_pass_contacts.get().x;
		debug_colors.get().array_up_pass_contacts[1] = debug_colors.get().up_pass_contacts.get().y;
		debug_colors.get().array_up_pass_contacts[2] = debug_colors.get().up_pass_contacts.get().z;
		debug_colors.get().array_up_pass_contacts[3] = debug_colors.get().up_pass_contacts.get().w;

		// side pass contacts
		debug_colors.get().array_side_pass_contacts[0] = debug_colors.get().side_pass_contacts.get().x;
		debug_colors.get().array_side_pass_contacts[1] = debug_colors.get().side_pass_contacts.get().y;
		debug_colors.get().array_side_pass_contacts[2] = debug_colors.get().side_pass_contacts.get().z;
		debug_colors.get().array_side_pass_contacts[3] = debug_colors.get().side_pass_contacts.get().w;

		// down pass contacts
		debug_colors.get().array_down_pass_contacts[0] = debug_colors.get().down_pass_contacts.get().x;
		debug_colors.get().array_down_pass_contacts[1] = debug_colors.get().down_pass_contacts.get().y;
		debug_colors.get().array_down_pass_contacts[2] = debug_colors.get().down_pass_contacts.get().z;
		debug_colors.get().array_down_pass_contacts[3] = debug_colors.get().down_pass_contacts.get().w;

		// is ground
		debug_colors.get().array_is_ground[0] = debug_colors.get().is_ground.get().x;
		debug_colors.get().array_is_ground[1] = debug_colors.get().is_ground.get().y;
		debug_colors.get().array_is_ground[2] = debug_colors.get().is_ground.get().z;
		debug_colors.get().array_is_ground[3] = debug_colors.get().is_ground.get().w;

		// is ceiling
		debug_colors.get().array_is_ceiling[0] = debug_colors.get().is_ceiling.get().x;
		debug_colors.get().array_is_ceiling[1] = debug_colors.get().is_ceiling.get().y;
		debug_colors.get().array_is_ceiling[2] = debug_colors.get().is_ceiling.get().z;
		debug_colors.get().array_is_ceiling[3] = debug_colors.get().is_ceiling.get().w;

		// is crouch
		debug_colors.get().array_is_crouch[0] = debug_colors.get().is_crouch.get().x;
		debug_colors.get().array_is_crouch[1] = debug_colors.get().is_crouch.get().y;
		debug_colors.get().array_is_crouch[2] = debug_colors.get().is_crouch.get().z;
		debug_colors.get().array_is_crouch[3] = debug_colors.get().is_crouch.get().w;

		// average speed
		debug_colors.get().array_average_speed[0] = debug_colors.get().average_speed.get().x;
		debug_colors.get().array_average_speed[1] = debug_colors.get().average_speed.get().y;
		debug_colors.get().array_average_speed[2] = debug_colors.get().average_speed.get().z;
		debug_colors.get().array_average_speed[3] = debug_colors.get().average_speed.get().w;

		// profile auto stepping
		debug_colors.get().array_auto_stepping[0] = debug_colors.get().auto_stepping.get().x;
		debug_colors.get().array_auto_stepping[1] = debug_colors.get().auto_stepping.get().y;
		debug_colors.get().array_auto_stepping[2] = debug_colors.get().auto_stepping.get().z;
		debug_colors.get().array_auto_stepping[3] = debug_colors.get().auto_stepping.get().w;
	}

	last_player_position = world_transform.getTranslate();
}

void FirstPersonController::update_debug()
{
	if (!is_initialized)
		return;

	// debug camera
	if (debug_camera.get().enabled)
	{
		if (!debug_camera.get().use_fixed_angles)
		{
			if (Input::isKeyPressed(Input::KEY_UP))
				debug_camera.get().vertical_angle += debug_camera.get().angular_speed * Game::getIFps();
			if (Input::isKeyPressed(Input::KEY_DOWN))
				debug_camera.get().vertical_angle -= debug_camera.get().angular_speed * Game::getIFps();

			debug_camera.get().vertical_angle = clamp(debug_camera.get().vertical_angle, -89.9f, 89.9f);

			if (Input::isKeyPressed(Input::KEY_RIGHT))
				debug_camera.get().horizontal_angle -= debug_camera.get().angular_speed * Game::getIFps();
			if (Input::isKeyPressed(Input::KEY_LEFT))
				debug_camera.get().horizontal_angle += debug_camera.get().angular_speed * Game::getIFps();

			if (debug_camera.get().horizontal_angle < -180.0f || 180.0f < debug_camera.get().horizontal_angle)
				debug_camera.get().horizontal_angle -= sign(debug_camera.get().horizontal_angle) * 360.0f;
		}

		if (Input::isKeyPressed(Input::KEY_EQUALS))
			debug_camera.get().distance -= debug_camera.get().zoom_speed * Game::getIFps();
		if (Input::isKeyPressed(Input::KEY_MINUS))
			debug_camera.get().distance += debug_camera.get().zoom_speed * Game::getIFps();

		debug_camera.get().distance = clamp(debug_camera.get().distance, 0.0f, debug_camera.get().max_distance);

		vec3 camera_direction = debug_camera.get().camera->getDirection();
		if (debug_camera.get().use_fixed_angles && camera)
		{
			if (dot(camera->getDirection(), vec3_down) < 1.0f)
				camera_direction = camera->getWorldDirection();
		} else
		{
			camera_direction = vec3_forward * rotateZ(debug_camera.get().horizontal_angle);
			camera_direction = camera_direction * rotate(cross(camera_direction, vec3_up), debug_camera.get().vertical_angle);
		}

		debug_camera.get().camera->setWorldDirection(camera_direction, vec3_up);
	debug_camera.get().camera->setWorldPosition((world_transform.getTranslate() + Vec3(vec3_up)
		* static_cast<Scalar>(shape->getRadius() + shape->getHeight() * 0.5f)) - Vec3(camera_direction)
		* static_cast<Scalar>(debug_camera.get().distance));
	}

	// use visualizer
	if (debug_visualizer.get().enabled)
	{
		if (debug_visualizer.get().player_shape)
			shape->renderVisualizer(debug_colors.get().player_shape);

		if (debug_visualizer.get().player_direction)
		{
			Vec3 p0 = world_transform.getTranslate() + Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f);
			Vec3 p1 = p0 + Vec3(world_transform.getAxisY());
			Visualizer::renderVector(p0, p1, debug_colors.get().player_direction);
		}

		if (debug_visualizer.get().camera && camera)
		{
			Vec3 p0 = camera->getWorldPosition();
			Vec3 p1 = p0 + Vec3(camera->getWorldDirection());
			Visualizer::renderVector(p0, p1, debug_colors.get().camera_color);

			camera->renderVisualizer();
		}

		if (debug_visualizer.get().slope_basis)
		{
			Vec3 p0 = world_transform.getTranslate();
			Visualizer::renderVector(p0, p0 + slope_axis_x, vec4_red);
			Visualizer::renderVector(p0, p0 + slope_axis_y, vec4_green);
			Visualizer::renderVector(p0, p0 + slope_normal, vec4_blue);
		}

		if (debug_visualizer.get().applied_horizontal_velocity)
		{
			Vec3 p0 = world_transform.getTranslate() + Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f);
			Vec3 p1 = p0 + horizontal_velocity;
			Visualizer::renderVector(p0, p1, debug_colors.get().applied_horizontal_velocity);
		}

		if (debug_visualizer.get().applied_vertical_velocity)
		{
			Vec3 p0 = world_transform.getTranslate() + Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f);
			Vec3 p1 = p0 + Vec3(vec3_up) * static_cast<Scalar>(vertical_velocity);
			Visualizer::renderVector(p0, p1, debug_colors.get().applied_vertical_velocity);
		}
	}

	// use profiler
	if (debug_profiler.get().enabled)
	{
		if (debug_profiler.get().applied_horizontal_speed)
			Profiler::setValue("Applied Horizontal Speed", "m/s", (float)horizontal_velocity.length(), max_applied_horizontal_speed, debug_colors.get().array_applied_horizontal_velocity);

		if (debug_profiler.get().applied_vertical_speed)
			Profiler::setValue("|Applied Vertical Speed|", "m/s", Math::abs(vertical_velocity), max_applied_vertical_speed, debug_colors.get().array_applied_vertical_velocity);

		if (debug_profiler.get().is_ground)
			Profiler::setValue("Is Ground", "", (is_ground ? 1.0f : 0.0f), max_flag_value, debug_colors.get().array_is_ground);

		if (debug_profiler.get().is_ceiling)
			Profiler::setValue("Is Ceiling", "", (is_ceiling ? 1.0f : 0.0f), max_flag_value, debug_colors.get().array_is_ceiling);

		if (debug_profiler.get().is_crouch)
			Profiler::setValue("Is Crouch", "", (is_crouch ? 1.0f : 0.0f), max_flag_value, debug_colors.get().array_is_crouch);

		if (debug_profiler.get().average_speed)
		{
			for (int i = 0; i < speeds_buffer_size - 1; i++)
				speeds_buffer[i] = speeds_buffer[i + 1];

			speeds_buffer[speeds_buffer_size - 1] = (float)(world_transform.getTranslate() - last_player_position).length() / Game::getIFps();
			last_player_position = world_transform.getTranslate();

			float avg_speed = 0.0f;
			for (int i = 0; i < speeds_buffer_size; i++)
				avg_speed += speeds_buffer[i];

			avg_speed /= (float)speeds_buffer_size;

			Profiler::setValue("Avg Speed", "m/s", avg_speed, max_applied_horizontal_speed * 1.75f, debug_colors.get().array_average_speed);
		}

		if (debug_profiler.get().auto_stepping)
			Profiler::setValue("Auto Stepping", "", (auto_stepping_applied ? 1.0f : 0.0f), max_flag_value, debug_colors.get().array_auto_stepping);
	}
}

#endif