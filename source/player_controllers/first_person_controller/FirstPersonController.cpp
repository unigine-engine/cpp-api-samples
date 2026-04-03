// Full-featured first-person character controller with capsule collision,
// walking/running/crouching, jumping, auto-stepping on stairs, slope handling,
// and physics object interaction. Uses custom collision resolution for
// frame-rate independent movement and stable ground detection.

#include "FirstPersonController.h"
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <UnigineProfiler.h>
#include <UnigineConsole.h>
#include <UnigineInput.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(FirstPersonController)

// Sets player transform from external source (e.g., teleportation).
// Constrains orientation to vertical (no tilt), extracts camera angles from matrix.
void FirstPersonController::setWorldTransform(const Unigine::Math::Mat4 &t)
{
	// Flatten Y axis to horizontal plane - player must remain upright
	Vec3 axis_y = t.getAxisY();
	axis_y.z = 0;
	axis_y = (axis_y.length2() > Consts::EPS ? axis_y.normalize() : Vec3_forward);
	node->setWorldTransform(setTo(t.getTranslate(), t.getTranslate() + axis_y, vec3_up, AXIS_Y));

	// Extract pitch and yaw from transform matrix and apply to camera
	if (camera && camera_mode != CameraMode::NONE)
	{
		// Vertical angle: derived from Z axis tilt relative to down vector
		camera_vertical_angle = getAngle(vec3_down, static_cast<vec3>(-t.getAxisZ()));
		camera_vertical_angle = clamp(camera_vertical_angle, min_vertical_angle + 90.0f, max_vertical_angle + 90.0f);

		// Horizontal angle: rotation around up axis
		camera_horizontal_angle = node->getWorldRotation().getAngle(vec3_up);

		// Reconstruct camera direction from spherical angles
		vec3 camera_direction = vec3_forward * rotateZ(-camera_horizontal_angle);
		camera_direction = camera_direction * rotate(cross(camera_direction, vec3_up), 90.0f - camera_vertical_angle);
		camera_direction.normalize();
		camera->setWorldDirection(camera_direction, vec3_up);
	}
}

// Initializes controller: creates/validates physics body and shape, sets up camera,
// precomputes angle cosines, and detects gamepad. Component must be attached to Object.
void FirstPersonController::init()
{
	// Component requires an Object node (not just Node) for physics body attachment
	ObjectPtr obj = checked_ptr_cast<Object>(node);
	if (!obj)
	{
		Log::error("FirstPersonController: can't cast node to Object\n");
		return;
	}

	// Enforce upright orientation: flatten Y axis to horizontal plane.
	// This prevents tilted characters and ensures consistent movement behavior.
	Vec3 axis_y = obj->getWorldTransform().getAxisY();
	axis_y.z = 0;
	axis_y = (axis_y.length2() > Consts::EPS ? axis_y.normalize() : Vec3_forward);
	obj->setWorldTransform(setTo(node->getWorldPosition(), node->getWorldPosition() + axis_y, vec3_up, AXIS_Y));

	// BodyDummy is used for manual collision handling (not physics simulation).
	// If use_object_body is true, try to use existing body from editor.
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

	// Capsule shape is ideal for character collision: rounded ends prevent
	// getting stuck on edges, cylindrical body allows smooth wall sliding.
	if (use_object_body)
	{
		// Search existing shapes for a capsule
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

	// Create default capsule if none found
	if (!shape)
	{
		shape = ShapeCapsule::create(body, capsule_radius, capsule_height);
		// Position capsule so bottom touches ground at player origin
		body->setShapeTransform(body->getNumShapes() - 1, translate(vec3_up * (capsule_radius + 0.5f * capsule_height)));

		shape->setPhysicsIntersectionMask(physics_intersection_mask);
		shape->setCollisionMask(collision_mask);
		shape->setExclusionMask(exclusion_mask);
	}

	// Cache actual capsule dimensions (may differ from parameters if using custom shape)
	capsule_height = shape->getHeight();
	// Crouch height must be at least diameter (two hemispheres) and at most full standing height
	crouch_height = clamp(crouch_height, 2.0f * shape->getRadius(), shape->getHeight() + 2.0f * shape->getRadius());
	// Initialize crouch state machine at standing height
	crouch_state.current_height = shape->getHeight() + 2.0f * shape->getRadius();
	crouch_state.start_height = crouch_state.current_height;
	crouch_state.end_height = crouch_height.get();

	// Camera can be: NONE (no management), CREATE_AUTOMATICALLY (child PlayerDummy),
	// or USE_EXTERNAL (editor-assigned camera node)
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

	// Cylinder shape for detecting rigid bodies to push. Slightly larger than capsule
	// to ensure contact detection even when not perfectly aligned.
	if (use_objects_interaction)
	{
		interaction_shape = ShapeCylinder::create();
		interaction_shape->setEnabled(false);
	}

	// Precompute cosines once for fast angle comparisons in collision checks.
	// Comparing cosines is faster than computing angles each frame.
	player_ifps = 1.0f / player_fps;
	crouch_transition_time = max(crouch_transition_time, Consts::EPS);

	cos_ground_angle = Math::cos(max_ground_angle * Math::Consts::DEG2RAD);
	cos_ceiling_angle = Math::cos(max_ceiling_angle * Math::Consts::DEG2RAD);
	cos_stair_angle = Math::cos(max_stair_angle * Math::Consts::DEG2RAD);
	cos_stairs_detection_angle = Math::cos(stairs_detection_angle * Math::Consts::DEG2RAD + Math::Consts::PI05);
	cos_wall_stop_sliding_angle = Math::cos(wall_stop_sliding_angle * Math::Consts::DEG2RAD);

	// Reusable intersection result object (avoids allocation each frame)
	normal_intersection = WorldIntersectionNormal::create();

#if DEBUG
	max_pass_contacts = contacts_buffer_size;
#endif

	world_transform = obj->getWorldTransform();

	// Find first available gamepad for alternative input
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

// Main update loop: reads input, updates physics in sub-steps, handles crouching.
// Called every render frame. Uses adaptive time stepping for consistent physics
// regardless of frame rate (subdivides frame time into player_ifps chunks).
void FirstPersonController::update()
{
	if (!is_initialized)
		return;

	world_transform = node->getWorldTransform();

	// Scale frame time by physics scale for slow-motion support
	float ifps = Game::getIFps() * Physics::getScale();

	update_move_directions(ifps);
	check_move_and_stair();

	// If movement is blocked and we have ground contacts, stop completely
	// to prevent jittering against walls
	if (!is_available_side_move)
	{
		if (has_bottom_contacts)
		{
			horizontal_velocity = Vec3_zero;
			horizontal_move_direction = vec2_zero;
		}
	}

	// Subdivide frame into fixed-size steps for consistent physics behavior.
	// Long frames are split into multiple iterations to prevent tunneling
	// and ensure stable collision resolution.
	float update_time = ifps;
	has_bottom_contacts = false;
	while (update_time > 0.0f)
	{
		float adaptive_time_step = min(update_time, player_ifps);
		update_time -= adaptive_time_step;

		update_velocity(adaptive_time_step, adaptive_time_step / ifps);
		// Apply velocity to transform, then resolve collisions
		mul(world_transform, translate((horizontal_velocity + Vec3_up * vertical_velocity) * adaptive_time_step), world_transform);
		update_collisions(adaptive_time_step);
	}

	update_crouch(ifps);

	// Apply final player transformation
	node->setWorldTransform(world_transform);
	body->setTransform(node->getWorldTransform());
	if (is_crouch)
		shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

	update_camera();

#if DEBUG
	update_debug();
#endif
}

// Physics tick: applies impulses to rigid bodies the player is touching.
// Runs at fixed physics rate (not render rate) for consistent object pushing.
void FirstPersonController::update_physics()
{
	if (!is_initialized)
		return;

	if (use_objects_interaction)
	{
		// Cylinder shape is slightly larger than capsule to catch objects
		// that might slip through gaps, especially at player's feet
		interaction_shape->setEnabled(true);
		interaction_shape->setRadius(shape->getRadius() + check_move_ray_offset + skin_width_offset);
		interaction_shape->setHeight(shape->getHeight() + 2.0f * (shape->getRadius() + check_move_ray_offset - skin_width_offset));
		interaction_shape->setTransform(translate(world_transform.getTranslate() + Vec3_up * (0.5f * interaction_shape->getHeight() + skin_width_offset)));

		interaction_shape->getCollision(contacts);
		int contacts_count = min(contacts_buffer_size, contacts.size());

		Scalar speed = horizontal_velocity.length() + Math::abs(vertical_velocity);
		speed = max(speed, static_cast<Scalar>(1.0f));

		// Calculate impulse strength based on player mass and speed.
		// Divided by contact count to distribute force evenly across all touched objects.
		float k = player_mass * impulse_multiplier * 0.5f;
		if (contacts_count > 0)
			k /= contacts_count;

		// Apply impulses to each contacted rigid body
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

// Reads keyboard/gamepad input and converts to movement direction vectors.
// Gamepad stick overrides keyboard if its input magnitude is larger.
void FirstPersonController::update_move_directions(float ifps)
{
	// Clear previous frame's input
	horizontal_move_direction = vec2_zero;
	vertical_move_direction = 0.0f;

	// Only process input when mouse is grabbed (game has focus)
	if (!Input::isMouseGrab())
		return;

	// Build direction vector from WASD keys (normalized diagonal movement)
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

	// Update vertical direction (jump impulse when grounded)
	if (use_jump && is_ground && (Input::isKeyDown(static_cast<Input::KEY>(jump_key.get())) ||
		game_pad && game_pad->isButtonDown(static_cast<Input::GAMEPAD_BUTTON>(jump_button.get()))))
	{
		vertical_move_direction = (is_crouch ? crouch_jump_power : jump_power) / ifps;
	}
}

// Raycasts ahead of player to detect walkable surfaces and stairs.
// Sets is_available_side_move if movement direction leads to valid ground,
// and is_available_stair if auto-stepping should be attempted.
void FirstPersonController::check_move_and_stair()
{
	is_available_side_move = false;
	is_available_stair = false;

	vec3 horizontal_direction = world_transform.getRotate() * vec3(horizontal_move_direction);
	horizontal_move_direction.normalize();

	// Raycast downward ahead of player to check surface walkability
	if (horizontal_move_direction.length2() > 0.0f)
	{
		Vec3 p2 = world_transform.getTranslate() + Vec3(horizontal_direction) * (static_cast<Scalar>(shape->getRadius()) + check_move_ray_offset) + Vec3_down * Scalar(check_move_ray_offset.get());
		Vec3 p1 = p2 + Vec3_up * (max(shape->getRadius(), max_step_height) + check_move_ray_offset);

		auto hitObj = World::getIntersection(p1, p2, check_move_mask, normal_intersection);
		if (hitObj)
		{
			if (dot(vec3_up, normal_intersection->getNormal()) > cos_ground_angle)
				is_available_side_move = true;

			// Allow movement when approaching a different slope - this enables
			// climbing ramps and transitions between surfaces of different angles
			Scalar cos = dot(slope_normal, Vec3(normal_intersection->getNormal()));
			if (cos < large_epsilon)
				is_available_side_move = true;
		}
		else
		{
			// Allow movement in air (no surface hit)
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

	// Check stair surface angle for auto-stepping
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

// Updates player velocity based on input, slope, rotation, and physics.
// Builds a local coordinate system aligned to current slope for natural movement.
// Ground movement has instant direction change; air movement has inertia.
void FirstPersonController::update_velocity(float ifps, float update_part)
{
	// Build slope-aligned coordinate system. Must check collinearity because
	// cross product of parallel vectors is zero (degenerate case on flat ground)
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

	// Decompose velocity into slope basis for instant direction change on ground
	Vec3 horizontal_velocity_decomposition = Vec3_zero;
	if (is_ground)
	{
		horizontal_velocity_decomposition.x = dot(slope_axis_x, horizontal_velocity);
		horizontal_velocity_decomposition.y = dot(slope_axis_y, horizontal_velocity);
		horizontal_velocity_decomposition.z = dot(slope_normal, horizontal_velocity);
	}

	// Player rotation from mouse/gamepad input
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

	// On the ground, change velocity direction instantly (no inertia)
	if (is_ground)
	{
		// Recompute slope basis after rotation
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

		// Restore velocity in new slope basis
		horizontal_velocity = slope_axis_x * horizontal_velocity_decomposition.x +
			slope_axis_y * horizontal_velocity_decomposition.y +
			slope_normal * horizontal_velocity_decomposition.z;
	}

	// Add horizontal velocity in slope basis
	float acceleration = (is_ground ? ground_acceleration : air_acceleration);
	horizontal_velocity += slope_axis_x * horizontal_move_direction.x * acceleration * ifps;
	horizontal_velocity += slope_axis_y * horizontal_move_direction.y * acceleration * ifps;

	// Update vertical velocity (jump + gravity)
	vertical_velocity += vertical_move_direction * ifps;
	if (!is_ground)
		vertical_velocity += Physics::getGravity().z * ifps;

	// Get current max speed based on movement mode
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

	// Apply damping when velocity exceeds target or no input is pressed
	vec2 targetSpeed = horizontal_move_direction * max_speed;
	if (targetSpeed.length() < Consts::EPS || targetSpeed.length() < horizontal_velocity.length())
		horizontal_velocity *= Math::exp((is_ground ? -ground_damping : -air_damping) * ifps);

	// Clamp horizontal velocity to max speed
	if (horizontal_velocity.length() > max_speed)
		horizontal_velocity = horizontal_velocity.normalize() * max_speed;

	// Check frozen state (prevents slope sliding when nearly stationary)
	is_horizontal_frozen = is_ground && (horizontal_velocity.length() < Physics::getFrozenLinearVelocity());
}

// Main collision resolution loop. Runs multiple iterations to handle complex
// contact scenarios (corners, wedges). Each iteration may auto-step up obstacles,
// resolve horizontal contacts, then step down to find ground.
void FirstPersonController::update_collisions(float ifps)
{
	// Reset collision state for this frame
	is_ground = false;
	is_ceiling = false;
	slope_normal = Vec3_up;

	is_heavy_contacts = false;

	// Resolve collisions iteratively
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

// Auto-stepping phase 1: elevate player over obstacle. Analyzes contacts
// to find the highest step in movement direction, then lifts player above it.
// Only activates when moving forward, not frozen, and falling (not jumping).
void FirstPersonController::try_move_up(float ifps)
{
	used_auto_stepping = false;
	last_step_height = 0.0f;

	// Only step up when moving horizontally and not jumping upward
	if (horizontal_move_direction.length2() > 0.0f && !is_horizontal_frozen && vertical_velocity < 0.0f)
	{
		body->setTransform(world_transform);
		if (is_crouch)
			shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

		// Query all contacts in current position
		shape->getCollision(contacts);
		if (contacts.size() == 0)
			return;

		if (contacts.size() > heavy_contacts_count)
			is_heavy_contacts = true;

		int contacts_count = min(contacts_buffer_size, contacts.size());

		// Find max step height from contacts in movement direction
		Vec2 velocity_xy = Vec2(horizontal_velocity);
		if (velocity_xy.length2() < auto_stepping_speed_threshold)
		{
			// Set minimal velocity for climb
			velocity_xy = Vec2(world_transform.getRotate() * Vec3(Vec2(horizontal_move_direction)));
			velocity_xy.normalize();
			horizontal_velocity = Vec3(velocity_xy * static_cast<Scalar>(walk_speed));
		}

		for (int i = 0; i < contacts_count; i++)
		{
			const ShapeContactPtr &c = contacts[i];

			Vec2 normalXY = Vec2(Vec3(c->getNormal()));

			// Skip contacts opposite to movement direction
			if (dot(normalXY, velocity_xy) > cos_stairs_detection_angle)
				continue;

			Scalar step = dot(c->getPoint() - world_transform.getTranslate(), Vec3_up);
			if (last_step_height < step)
				last_step_height = step;
		}

		// Apply auto-stepping if step height is within valid range
		if (min_step_height < last_step_height && last_step_height < max_step_height)
		{
			world_transform.setColumn3(3, world_transform.getTranslate() + Vec3(vec3_up) * last_step_height);

			// Check contacts after elevating; cancel step if blocked
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

// Main collision pass: resolves penetrations with world geometry.
// Pushes player out of contacts, cancels velocity into surfaces, detects
// ground/ceiling states, and tracks slope normal for movement direction.
void FirstPersonController::move_side(float ifps)
{
	// Sync physics body with current world transform
	body->setTransform(world_transform);
	if (is_crouch)
		shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

	// Get contacts in new position and process them
	shape->getCollision(contacts);
	if (contacts.size() == 0)
		return;

	if (contacts.size() > heavy_contacts_count)
		is_heavy_contacts = true;

	int contacts_count = min(contacts_buffer_size, contacts.size());

	// Total position offset accumulated from all contact depths
	auto position_offset = vec3_zero;

	// Track maximum slope angle under player
	float max_cos_angle = 1.0f;

	float inum = 1.0f / contacts_count;
	for (int i = 0; i < contacts_count; i++)
	{
		const ShapeContactPtr &c = contacts[i];

		// When frozen, move only vertically to prevent slope sliding;
		// otherwise resolve in all directions
		if (is_horizontal_frozen)
		{
			float depth = dot(vec3_up, c->getNormal()) * (c->getDepth() - Consts::EPS);
			position_offset += vec3_up * depth * inum;
		}
		else
		{
			position_offset += c->getNormal() * (c->getDepth() - Consts::EPS) * inum;

			// Remove velocity component projected onto contact normal
			Scalar normal_speed = dot(Vec3(c->getNormal()), horizontal_velocity);
			horizontal_velocity -= Vec3(c->getNormal()) * normal_speed;
		}

		// Stop sliding when hitting wall nearly head-on
		if ((c->getObject() && !c->getObject()->getBodyRigid()) && shape->getBottomCap().z < c->getPoint().z && c->getPoint().z < shape->getTopCap().z)
		{
			float cos = dot(world_transform.getRotate() * vec3(horizontal_move_direction), -c->getNormal());
			if (cos > cos_wall_stop_sliding_angle)
				horizontal_velocity = Vec3_zero;
		}

		// Check ground state: contact on bottom hemisphere + surface angle within limit
		if (dot(c->getPoint() - shape->getBottomCap(), Vec3_up) < 0.0f)
		{
			has_bottom_contacts = true;
			if (dot(c->getNormal(), vec3_up) > cos_ground_angle)
			{
				vertical_velocity = Physics::getGravity().z * ifps;
				is_ground = true;
			}

			// Track steepest surface normal under player
			float cos_angle = dot(vec3_up, c->getNormal());
			if (compare(cos_angle, 0.0f, 0.01f) == 0 && cos_angle < max_cos_angle)
			{
				slope_normal = Vec3(contacts[i]->getNormal());
				max_cos_angle = cos_angle;
			}
		}

		// Check ceiling state: contact on top hemisphere + surface angle within limit
		if (dot(contacts[i]->getNormal(), vec3_down) > cos_ceiling_angle && dot(contacts[i]->getPoint() - shape->getTopCap(), Vec3_down) < 0.0f)
		{
			is_ceiling = true;

			// Stop upward movement on ceiling hit
			vertical_velocity = 0.0f;
		}
	}

	// Apply accumulated position offset
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

// Auto-stepping phase 2: drop player back down after elevation.
// After try_move_up lifted us over an obstacle, this finds the actual
// ground level and snaps player to it for smooth stair traversal.
void FirstPersonController::try_move_down(float ifps)
{
	// Large steps cause jittering because player repeatedly overshoots and
	// corrects. Clamping to capsule radius ensures smooth motion on big stairs.
	if (last_step_height > shape->getRadius())
		last_step_height = shape->getRadius() - Physics::getPenetrationTolerance();

	// Drop player by the step height we elevated in try_move_up
	world_transform.setColumn3(3, world_transform.getTranslate() - Vec3(vec3_up) * last_step_height);

	body->setTransform(world_transform);
	if (is_crouch)
		shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + shape->getHeight() * 0.5f)));

	// Find collisions with the capsule
	shape->getCollision(contacts);
	if (contacts.size() == 0)
		return;

	if (contacts.size() > heavy_contacts_count)
		is_heavy_contacts = true;

	int contacts_count = min(contacts_buffer_size, contacts.size());

	// Resolve contacts to find ground
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

// State machine for smooth crouch transitions. Handles four phases:
// STAND (full height), MOVE_DOWN (shrinking), CROUCH (crouched), MOVE_UP (growing).
// Checks overhead clearance before standing to prevent clipping through ceilings.
void FirstPersonController::update_crouch(float ifps)
{
	if (!use_crouch)
		return;

	// Read crouch input from keyboard or gamepad
	bool is_key = (Input::isKeyPressed(static_cast<Input::KEY>(crouch_key.get())) ||
				  game_pad && game_pad->isButtonPressed(static_cast<Input::GAMEPAD_BUTTON>(crouch_button.get())));

	// State machine: determine behavior based on current crouch phase
	switch (crouch_state.phase)
	{
		case CrouchPhase::STAND:
			if (is_key)
			{
				// Begin smooth transition to crouch
				crouch_state.phase = CrouchPhase::MOVE_DOWN;
				swap_interpolation_direction(capsule_height + 2.0f * shape->getRadius(), crouch_height);
				is_crouch = true;
			}
			break;

		case CrouchPhase::MOVE_DOWN:
		case CrouchPhase::CROUCH:
			if (!is_key)
			{
				// Try to stand up: check if there's room overhead
				bool can_stand_up = true;

				// Temporarily set standing shape (narrower to avoid false wall contacts)
				float radius = shape->getRadius();
				shape->setRadius(radius - skin_width_offset);
				update_player_height(capsule_height + 2.0f * skin_width_offset);

				// Check for overhead obstructions
				shape->getCollision(contacts);

				Scalar top_point = world_transform.getTranslate().z + crouch_height;
				for (int i = 0; i < contacts.size(); i++)
					if (contacts[i]->getPoint().z > top_point)
					{
						// Blocked by overhead obstacle
						can_stand_up = false;
						break;
					}

				// Restore current shape parameters
				shape->setRadius(radius);
				update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());

				if (can_stand_up)
				{
					// Begin smooth transition to standing
					crouch_state.phase = CrouchPhase::MOVE_UP;
					swap_interpolation_direction(crouch_height, capsule_height + 2.0f * shape->getRadius());
				}
			}
			break;

		case CrouchPhase::MOVE_UP:
			if (is_ceiling || is_key)
			{
				// Ceiling hit or key pressed: return to crouching
				crouch_state.phase = CrouchPhase::MOVE_DOWN;
				swap_interpolation_direction(capsule_height + 2.0f * shape->getRadius(), crouch_height);
			}
			break;

		default: break;
	}

	// Handle smooth height interpolation
	if (crouch_state.current_time > 0.0f)
	{
		// Compute lerp factor based on remaining transition time
		float t = 1.0f;
		if (compare(crouch_transition_time, Consts::EPS) == 0)
			t = saturate(1.0f - crouch_state.current_time / crouch_transition_time);

		// Update capsule height
		crouch_state.current_height = lerp(crouch_state.start_height, crouch_state.end_height, static_cast<Scalar>(t));
		update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());

		crouch_state.current_time -= ifps;

		// Finalize transition when time expires
		if (crouch_state.current_time <= 0.0f)
		{
			// Set final time and height
			crouch_state.current_time = 0.0f;
			crouch_state.current_height = crouch_state.end_height;

			switch (crouch_state.phase)
			{
				case CrouchPhase::MOVE_DOWN:
					// Transition complete: enter crouched state
					update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());
					crouch_state.phase = CrouchPhase::CROUCH;
					break;

				case CrouchPhase::MOVE_UP:
					// Transition complete: enter standing state
					update_player_height(crouch_state.current_height - 2.0f * shape->getRadius());
					crouch_state.phase = CrouchPhase::STAND;
					is_crouch = false;
					break;

				default: break;
			}
		}
	}
}

// Updates camera position and orientation. Applies vertical angle from mouse
// input, combines all position offsets (base, crouch, additional), and
// constructs view direction from spherical coordinates (yaw + pitch).
void FirstPersonController::update_camera()
{
	if (!camera || camera_mode == CameraMode::NONE)
		return;

	if (Input::isMouseGrab())
	{
		// Read vertical look from mouse or gamepad (whichever has larger delta)
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

	// Apply camera position with all offsets (base + crouch + additional)
	camera->setWorldPosition(world_transform * (Vec3(camera_position_offset.get()) + Vec3(camera_crouch_offset) + Vec3(additional_camera_offset)));

	vec3 camera_direction = vec3_forward * rotateZ(-camera_horizontal_angle);
	camera_direction = camera_direction * rotate(cross(camera_direction, vec3_up), 90.0f - camera_vertical_angle);
	camera_direction = additional_camera_rotation * camera_direction;
	camera_direction.normalize();
	camera->setWorldDirection(camera_direction, vec3_up);
}

// Reverses crouch transition direction mid-interpolation. Used when player
// releases crouch key while still transitioning down, or hits ceiling while
// transitioning up. Preserves smooth motion by using remaining time.
void FirstPersonController::swap_interpolation_direction(Scalar start_height, Scalar end_height)
{
	// Remaining transition time becomes new duration (smooth reversal)
	crouch_state.current_time = max(Consts::EPS, crouch_transition_time - crouch_state.current_time);
	crouch_state.start_height = start_height;
	crouch_state.end_height = end_height;
}

// Resizes collision capsule and adjusts camera offset to match new height.
// Called during crouch transitions to smoothly animate player height.
void FirstPersonController::update_player_height(Scalar height)
{
	shape->setHeight((float)height);
	// Camera offset is difference between current and standing height
	camera_crouch_offset = vec3_up * (height - capsule_height);
	// Re-center capsule shape on player origin
	shape->setTransform(world_transform * translate(Vec3_up * (shape->getRadius() + height * 0.5f)));
}

#if DEBUG

// Initializes debug visualization systems: third-person camera for observing
// player externally, visualizer for rendering collision data, and profiler
// for displaying real-time metrics on screen.
void FirstPersonController::init_debug()
{
	if (!is_initialized)
		return;

	// Create orbiting debug camera that looks at player from outside
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

	// Enable visualizer for debug rendering
	String command;

	Visualizer::setEnabled(debug_visualizer.get().enabled != 0);
	if (debug_visualizer.get().enabled)
	{
		Render::setShowTriangles(debug_visualizer.get().triangles ? 1 : 0);

		int showShapes = (debug_visualizer.get().shapes ? 1 : 0);
		command = "physics_show_shapes " + String::itoa(showShapes);
		Console::run(command);
	}

	// Enable profiler for performance metrics
	int show_profiler = (debug_profiler.get().enabled ? 1 : 0);
	command = "show_profiler " + String::itoa(show_profiler);
	Console::run(command);
	if (debug_profiler.get().enabled)
	{
		// Initialize profiler color arrays for each metric
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

// Per-frame debug update: controls debug camera orbit via arrow keys,
// renders visualizations (shape, velocities, contacts), and updates
// profiler metrics (speeds, states, contact counts).
void FirstPersonController::update_debug()
{
	if (!is_initialized)
		return;

	// Arrow keys orbit debug camera around player, +/- zoom in/out
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

	// Render debug visualizations
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

	// Update profiler metrics
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