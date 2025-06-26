#include "StateMachine.h"

#include <UnigineConsole.h>
#include <UnigineGame.h>
#include <UnigineEngine.h>

using namespace Unigine;
using namespace Math;

namespace AnimationSystem
{

UNIGINE_INLINE mat4 getTransformProj(const mat4 &t, AXIS axis_forward)
{
	vec3 forward_axis;
	switch (axis_forward) {
		case AXIS_X: forward_axis = t.getAxisX(); break;
		case AXIS_Y: forward_axis = t.getAxisY(); break;
		case AXIS_Z: forward_axis = t.getAxisZ(); break;
		case AXIS_NX: forward_axis = -t.getAxisX(); break;
		case AXIS_NY: forward_axis = -t.getAxisY(); break;
		case AXIS_NZ: forward_axis = -t.getAxisZ(); break;
	}
	vec3 position = projectOntoPlane(t.getTranslate(), vec3_up);
	forward_axis = projectOntoPlane(forward_axis, vec3_up);
	forward_axis.normalize();
	vec3 right_axis = cross(forward_axis, vec3_up);
	right_axis.normalize();

	mat4 proj_t;
	proj_t.setColumn3(0, right_axis);
	proj_t.setColumn3(1, forward_axis);
	proj_t.setColumn3(2, vec3_up);
	proj_t.setColumn3(3, position);
	return proj_t;
}

UNIGINE_INLINE mat4 getLayerBoneTransformProj(const ObjectMeshSkinnedPtr &skinned, int layer, int bone, AXIS axis_forward)
{
	const mat4 bone_t = skinned->getLayerBoneTransform(layer, bone);
	return getTransformProj(bone_t, axis_forward);
}

///////////////////////////////////////////////////////////////////////////////////
// Interpolator
///////////////////////////////////////////////////////////////////////////////////

void Interpolator::initSrcValues(const ObjectMeshSkinnedPtr &skinned)
{
	int num = skinned->getNumBones();

	src_positions.resize(num);
	src_rotations.resize(num);

	for (int i = 0; i < num; i++)
	{
		src_positions[i] = skinned->getLayerBonePosition(LAYER_RESULT, i);
		src_rotations[i] = skinned->getLayerBoneRotation(LAYER_RESULT, i);
	}
}

void Interpolator::initDstValues(const ObjectMeshSkinnedPtr &skinned)
{
	int num = skinned->getNumBones();

	dst_positions.resize(num);
	dst_rotations.resize(num);

	for (int i = 0; i < num; i++)
	{
		dst_positions[i] = skinned->getLayerBonePosition(LAYER_RESULT, i);
		dst_rotations[i] = skinned->getLayerBoneRotation(LAYER_RESULT, i);
	}
}

void Interpolator::calculateOffsets(float in_half_life)
{
	// don't accumulate offsets as this leads to problems when states change frequently
	// just resets them
	for (int i = 0; i < src_positions.size(); i++)
		offset_positions[i] = src_positions[i] - dst_positions[i];

	for (int i = 0; i < src_rotations.size(); i++)
		offset_rotations[i] = abs(src_rotations[i] * inverse(dst_rotations[i]));

	for (int i = 0; i < src_positions.size(); i++)
		offset_linear_velocities[i] = vec3_zero;

	for (int i = 0; i < src_positions.size(); i++)
		offset_angular_velocities[i] = vec3_zero;

	src_positions.clear();
	src_rotations.clear();
	dst_positions.clear();
	dst_rotations.clear();

	half_life = in_half_life;
	active_time = half_life * 10.0f;
}

void Interpolator::update(float ifps)
{
	if (active_time < 0.0f)
		return;
	active_time -= ifps;

	const float y = (4.0f * Consts::LOG2) / Math::max(half_life, Consts::EPS) / 2.0f;
	const float exp_value = Math::exp(-y * ifps);

	for (int i = 0; i < offset_positions.size(); i++)
	{
		// position offset
		{
			vec3 j1_ifps = (offset_linear_velocities[i] + offset_positions[i] * y) * ifps;

			offset_positions[i] = (offset_positions[i] + j1_ifps) * exp_value;
			offset_linear_velocities[i] = (offset_linear_velocities[i] - j1_ifps * y) * exp_value;
		}

		// rotation offset
		{
			vec3 j0 = log(offset_rotations[i]) * 2.0f;
			vec3 j1_ifps = (offset_angular_velocities[i] + j0 * y) * ifps;

			offset_rotations[i] = exp((j0 + j1_ifps) * exp_value * 0.5f);
			offset_angular_velocities[i] = (offset_angular_velocities[i] - j1_ifps * y) * exp_value;
		}
	}
}

void Interpolator::reset(const ObjectMeshSkinnedPtr &skinned)
{
	int num = skinned->getNumBones();
	offset_positions.resize(vec3_zero, num);
	offset_rotations.resize(quat_identity, num);
	offset_linear_velocities.resize(vec3_zero, num);
	offset_angular_velocities.resize(vec3_zero, num);

	src_positions.resize(vec3_zero, num);
	src_rotations.resize(quat_identity, num);

	dst_positions.resize(vec3_zero, num);
	dst_rotations.resize(quat_identity, num);

	active_time = 0.0f;
	half_life = 0.2f;
}

void Interpolator::applyCurrentOffsets(ObjectMeshSkinnedPtr &skinned)
{
	if (active_time < 0.0f)
		return;

	int num_bones = skinned->getNumBones();

	vec3 pos;
	quat rot;
	for (int i = 0; i < num_bones; i++)
	{
		pos = skinned->getLayerBonePosition(LAYER_RESULT, i);
		rot = skinned->getLayerBoneRotation(LAYER_RESULT, i);

		pos += offset_positions[i];
		rot = offset_rotations[i] * rot;

		skinned->setLayerBonePosition(LAYER_RESULT, i, pos);
		skinned->setLayerBoneRotation(LAYER_RESULT, i, rot);
	}
}

void Interpolator::clearOffsets()
{
	for (int i = 0; i < offset_positions.size(); i++)
	{
		offset_positions[i] = vec3_zero;
		offset_rotations[i] = quat_identity;
		offset_linear_velocities[i] = vec3_zero;
		offset_angular_velocities[i] = vec3_zero;
	}
}

///////////////////////////////////////////////////////////////////////////////////
// Transition
///////////////////////////////////////////////////////////////////////////////////

void Transition::parseConditions(const StateMachine &state_machine, const char *conditions_str)
{
	conditions.clear();

	auto condition_parts = String::split(conditions_str, "AND");
	for (int i = 0; i < condition_parts.size(); i++)
	{
		String condition_str = condition_parts[i];
		condition_str = condition_str.trim();
		Transition::Condition condition;

		if (String::compare(condition_str, "end") == 0)
		{
			condition.func = Transition::FUNC_END;
			conditions.append(condition);
			continue;
		}

		auto parts = String::split(condition_str, " ");
		if (parts.size() != 3)
			continue;

		if (state_machine.containsVariable(parts[0]) == false)
			continue;

		condition.var_type = state_machine.getVariableType(parts[0]);
		condition.var_id = state_machine.getVariableID(condition.var_type, parts[0]);

		if (String::compare(parts[1], "<") == 0)
		{
			condition.func = Transition::FUNC_LESS;
			condition.const_0 = String::atof(parts[2]);
			conditions.append(condition);
			continue;
		}

		if (String::compare(parts[1], ">") == 0)
		{
			condition.func = Transition::FUNC_GREATER;
			condition.const_0 = String::atof(parts[2]);
			conditions.append(condition);
			continue;
		}

		if (String::compare(parts[1], "==") == 0)
		{
			condition.func = Transition::FUNC_EQUAL;
			condition.const_0 = String::atof(parts[2]);
			conditions.append(condition);
			continue;
		}

		if (String::compare(parts[1], "!=") == 0)
		{
			condition.func = Transition::FUNC_NOT_EQUAL;
			condition.const_0 = String::atof(parts[2]);
			conditions.append(condition);
			continue;
		}

		if (String::compare(parts[1], "in") == 0)
		{
			auto values = String::split(parts[2], ";");
			if (values.size() == 2)
			{
				condition.func = Transition::FUNC_IN_RANGE;
				condition.const_0 = String::atof(values[0]);
				condition.const_1 = String::atof(values[1]);
				conditions.append(condition);
				continue;
			}
		}

		if (String::compare(parts[1], "out") == 0)
		{
			auto values = String::split(parts[2], ";");
			if (values.size() == 2)
			{
				condition.func = Transition::FUNC_OUT_RANGE;
				condition.const_0 = String::atof(values[0]);
				condition.const_1 = String::atof(values[1]);
				conditions.append(condition);
				continue;
			}
		}
	}
}

bool Transition::check(const StateMachine &state_machine, const State *state) const
{
	auto check_condition = [this, &state_machine, state](const Condition &condition)
	{
		if (condition.func == FUNC_END)
			return state->isEnd();

		if (condition.var_type == VAR_TYPE_NONE || condition.var_id == -1)
			return false;

		const float var_value = state_machine.getStateVariable(condition.var_type, condition.var_id);
		switch (condition.func)
		{
			case FUNC_GREATER:   return var_value > condition.const_0;
			case FUNC_LESS:      return var_value < condition.const_0;
			case FUNC_EQUAL:     return toBool(compare(var_value, condition.const_0));
			case FUNC_NOT_EQUAL: return toBool(compare(var_value, condition.const_0)) == false;
			case FUNC_IN_RANGE:  return (condition.const_0 < var_value && var_value < condition.const_1);
			case FUNC_OUT_RANGE: return (var_value < condition.const_0 || condition.const_1 < var_value);
		}

		return false;
	};

	for (const auto &condition : conditions)
	{
		if (check_condition(condition) == false)
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
// State
///////////////////////////////////////////////////////////////////////////////////

State::State(const char *name, int type, AXIS axis_forward)
	: name(name)
	, type(type)
	, axis_forward(axis_forward)
{}

void State::addTransition(const StateMachine &state_machine, const char *next_state, const char *conditions, float half_life)
{
	Transition &transition = transitions.append();
	transition.next_state = next_state;
	transition.parseConditions(state_machine, conditions);
	transition.half_life = half_life;
}

const char *State::checkTransition(const StateMachine &state_machine, float &out_transition_half_life)
{
	if (state_time < min_playback_time)
		return nullptr;

	out_transition_half_life = 0.0f;
	for (const auto &transition : transitions)
	{
		if (transition.check(state_machine, this))
		{
			out_transition_half_life = transition.half_life;
			return transition.next_state;
		}
	}

	return nullptr;
}

void State::init(const StateMachine &state_machine, ObjectMeshSkinnedPtr &skinned)
{
	if (animations_data.empty())
		return;

	switch (type)
	{
		case TYPE_ANIMATION:
		{
			// choose animation
			current_anim_index = 0;
			if (isUseRandomAnimation())
				current_anim_index = Game::getRandomInt(0, animations_data.size());

			// get num frames
			num_frames = animations_data[current_anim_index].animation->num_frames;

			resetForFrame(state_machine, skinned, 0.0f);
			break;
		}
		case TYPE_BLEND_1D:
		case TYPE_BLEND_2D:
		{
			// get num frames
			num_frames = animations_data[0].animation->num_frames;

			resetForFrame(state_machine, skinned, 0.0f);
			break;
		}
	}
}

void State::resetForFrame(const StateMachine &state_machine, ObjectMeshSkinnedPtr &skinned, float in_frame, int anim_index /*= -1*/)
{
	if (animations_data.empty())
		return;

	switch (type)
	{
		case TYPE_ANIMATION:
		{
			// set current animation
			if (anim_index != -1)
				current_anim_index = anim_index;

			// update num frames
			num_frames = animations_data[current_anim_index].animation->num_frames;

			// set frame and previous root transform
			setFrame(in_frame, state_machine, skinned);

			if (state_machine.isUseRootMotion())
			{
				animations_data[current_anim_index].prev_root_t = getLayerBoneTransformProj(skinned, LAYER_RESULT, state_machine.getRootBone(), axis_forward);
				animations_data[current_anim_index].root_motion_delta = mat4_identity;
			}

			break;
		}
		case TYPE_BLEND_1D:
		case TYPE_BLEND_2D:
		{
			// set frames and previous root transform
			if (anim_index == -1)
				anim_index = 0;

			float f = (in_frame / (animations_data[anim_index].animation->num_frames - 1)) * (getNumFrames() - 1);
			setFrame(f, state_machine, skinned);

			if (state_machine.isUseRootMotion())
			{
				const float normalized_frame = getNormalizedFrame();
				for (int i = 0; i < animations_data.size(); i++)
				{
					float f = normalized_frame * (animations_data[i].animation->num_frames - 1);

					skinned->setLayerAnimationResourceID(LAYER_AUXILIARY, animations_data[i].anim_id);
					skinned->setLayerFrame(LAYER_AUXILIARY, f);

					animations_data[i].prev_root_t = getLayerBoneTransformProj(skinned, LAYER_AUXILIARY, state_machine.getRootBone(), axis_forward);
					animations_data[current_anim_index].root_motion_delta = mat4_identity;
				}
			}

			break;
		}
	}

	root_motion_delta = mat4_identity;
}

void State::findBestFootAnimation(int &out_anim_index, int &out_frame, const StateMachine &state_machine,
								  const vec3 &left_foot_pos, const vec3 &right_foot_pos)
{
	out_anim_index = current_anim_index;
	out_frame = 0;

	if (animations_data.empty())
		return;

	auto get_frame_error = [this](const StateMachine &state_machine, int anim_index, int frame,
								  const vec3 &left_foot_pos, const vec3 &right_foot_pos)
	{
		const vec3 &dst_left_foot_pos = animations_data[anim_index].animation->left_foot_positions[frame];
		const vec3 &dst_right_foot_pos = animations_data[anim_index].animation->right_foot_positions[frame];

		float error = (dst_left_foot_pos - left_foot_pos).length2() + (dst_right_foot_pos - right_foot_pos).length2();
		return error;
	};

	float min_error = Consts::INF;
	if (foot_matching_override || type == TYPE_BLEND_1D || type == TYPE_BLEND_2D)
	{
		for (int i = 0; i < animations_data.size(); i++)
		{
			int num = animations_data[i].animation->num_frames;
			for (int j = ftoi(num * foot_matching_begin); j < ftoi(num * foot_matching_end); j++)
			{
				float error = get_frame_error(state_machine, i, j, left_foot_pos, right_foot_pos);
				if (error < min_error)
				{
					out_anim_index = i;
					out_frame = j;
					min_error = error;
				}
			}
		}
	} else
	{
		int num = animations_data[out_anim_index].animation->num_frames;
		for (int j = ftoi(num * foot_matching_begin); j < ftoi(num * foot_matching_end); j++)
		{
			float error = get_frame_error(state_machine, out_anim_index, j, left_foot_pos, right_foot_pos);
			if (error < min_error)
			{
				out_frame = j;
				min_error = error;
			}
		}
	}
}

void State::update(float ifps, float speed, const StateMachine &state_machine, Interpolator *interpolator,
				   ObjectMeshSkinnedPtr &skinned)
{
	switch (type)
	{
		case TYPE_ANIMATION: update_animation(ifps, speed, state_machine, skinned);  break;
		case TYPE_BLEND_1D:  update_blend_tree(ifps, speed, state_machine, skinned); break;
		case TYPE_BLEND_2D:  update_blend_tree(ifps, speed, state_machine, skinned); break;
	}

	state_time += ifps;
}

void State::setFrame(float in_frame, const StateMachine &state_machine, ObjectMeshSkinnedPtr &skinned)
{
	if (animations_data.empty())
		return;

	frame = clamp(in_frame, 0.0f, itof(getNumFrames() - 1));

	switch (type)
	{
		case TYPE_ANIMATION:
		{
			skinned->setLayerAnimationResourceID(LAYER_RESULT, animations_data[current_anim_index].anim_id);
			skinned->setLayerFrame(LAYER_RESULT, frame);

			int prev_index = floorInt(frame);
			int next_index = ceilInt(frame);
			float k = frac(frame);

			left_foot_pos = lerp(animations_data[current_anim_index].animation->left_foot_positions[prev_index],
								 animations_data[current_anim_index].animation->left_foot_positions[next_index], k);

			right_foot_pos = lerp(animations_data[current_anim_index].animation->right_foot_positions[prev_index],
								  animations_data[current_anim_index].animation->right_foot_positions[next_index], k);

			if (state_machine.isUseRootMotion())
			{
				// root motion delta
				mat4 t = getLayerBoneTransformProj(skinned, LAYER_RESULT, state_machine.getRootBone(), axis_forward);
				mat4 delta = inverse(animations_data[current_anim_index].prev_root_t) * t;

				animations_data[current_anim_index].root_motion_delta = delta * animations_data[current_anim_index].last_delta;
				animations_data[current_anim_index].prev_root_t = t;
			}

			break;
		}
		case TYPE_BLEND_1D:
		case TYPE_BLEND_2D:
		{
			if (getType() == TYPE_BLEND_1D)
			{
				float x = state_machine.getStateVariable(blend_x_var_type, blend_x_var_id);
				if (compare(x, blend_point.x) == false)
				{
					blend_point.x = x;
					update_weights_1d();
				}
			} else
			{
				float x = state_machine.getStateVariable(blend_x_var_type, blend_x_var_id);
				float y = state_machine.getStateVariable(blend_y_var_type, blend_y_var_id);
				if (compare(x, blend_point.x) == false || compare(y, blend_point.y) == false)
				{
					blend_point.x = x;
					blend_point.y = y;
					update_weights_2d();
				}
			}

			const float normalized_frame = getNormalizedFrame();

			float total_weight = 0.0f;
			for (int i = 0; i < animations_data.size(); i++)
			{
				if (animations_data[i].weight < Consts::EPS)
					continue;

				skinned->setLayerAnimationResourceID(LAYER_AUXILIARY, animations_data[i].anim_id);

				float f = normalized_frame * (animations_data[i].animation->num_frames - 1);
				skinned->setLayerFrame(LAYER_AUXILIARY, f);

				total_weight += animations_data[i].weight;
				skinned->lerpLayer(LAYER_RESULT, LAYER_RESULT, LAYER_AUXILIARY, animations_data[i].weight / total_weight);

				int prev_index = floorInt(f);
				int next_index = ceilInt(f);
				float k = frac(frame);

				vec3 left_pos(lerp(animations_data[i].animation->left_foot_positions[prev_index],
									 animations_data[i].animation->left_foot_positions[next_index], k));

				vec3 right_pos(lerp(animations_data[i].animation->right_foot_positions[prev_index],
									  animations_data[i].animation->right_foot_positions[next_index], k));

				left_foot_pos = lerp(left_foot_pos, left_pos, animations_data[i].weight / total_weight);
				right_foot_pos = lerp(right_foot_pos, right_pos, animations_data[i].weight / total_weight);

				if (state_machine.isUseRootMotion())
				{
					mat4 t = getLayerBoneTransformProj(skinned, LAYER_AUXILIARY, state_machine.getRootBone(), axis_forward);
					mat4 delta = inverse(animations_data[i].prev_root_t) * t;

					animations_data[i].root_motion_delta = delta * animations_data[i].last_delta;
					animations_data[i].prev_root_t = t;
				}
			}

			break;
		}
	}
}

void State::setFootMatching(float begin, float end, bool is_override)
{
	foot_matching_begin = begin;
	foot_matching_end = end;
	foot_matching_override = is_override;
}

void State::addAnimation(Animation *animation, const ObjectMeshSkinnedPtr &skinned)
{
	if (getType() != TYPE_ANIMATION)
		return;

	AnimData &data = animations_data.append();
	data.animation = animation;
	data.anim_id = skinned->getAnimationResourceID(data.animation->path);
}

void State::addBlend1DAnimation(Animation *animation, float anchor, const ObjectMeshSkinnedPtr &skinned)
{
	if (getType() != TYPE_BLEND_1D)
		return;

	AnimData &data = animations_data.append();
	data.animation = animation;
	data.anim_id = skinned->getAnimationResourceID(data.animation->path);
	data.anchor.x = anchor;

	min_anchors_value.x = min(min_anchors_value.x, data.anchor.x);
	max_anchors_value.x = max(max_anchors_value.x, data.anchor.x);

	quickSort(animations_data.begin(), animations_data.end(),
			  [](const AnimData &a, const AnimData &b) { return a.anchor.x < b.anchor.x; });
}

void State::addBlend2DAnimation(Animation *animation, const vec2 &anchor, const ObjectMeshSkinnedPtr &skinned)
{
	if (getType() != TYPE_BLEND_2D)
		return;

	AnimData &data = animations_data.append();
	data.animation = animation;
	data.anim_id = skinned->getAnimationResourceID(data.animation->path);
	data.anchor = anchor;

	min_anchors_value = min(min_anchors_value, data.anchor);
	max_anchors_value = max(max_anchors_value, data.anchor);
}

void State::update_animation(float ifps, float speed, const StateMachine &state_machine, ObjectMeshSkinnedPtr &skinned)
{
	if (animations_data.empty())
		return;

	// check end of playback
	if (isEnd())
	{
		root_motion_delta = mat4_identity;
		return;
	}

	// next frame
	frame += ifps * speed;
	frame = clamp(frame, 0.0f, itof(getNumFrames() - 1));

	const int last_index = getNumFrames() - 1;
	if (ftoi(frame) == last_index)
	{
		if (state_machine.isUseRootMotion())
		{
			// get delta from current position to last frame
			setFrame(itof(last_index), state_machine, skinned);
			animations_data[current_anim_index].last_delta = animations_data[current_anim_index].root_motion_delta;
		}

		if (isLoop())
		{
			frame -= itof(last_index);
			resetForFrame(state_machine, skinned, 0.0f);
		} else
		{
			frame = itof(last_index);
		}
	} else
	{
		if (state_machine.isUseRootMotion())
			animations_data[current_anim_index].last_delta = mat4_identity;
	}

	setFrame(frame, state_machine, skinned);

	if (state_machine.isUseRootMotion())
		root_motion_delta = animations_data[current_anim_index].root_motion_delta;
}

void State::update_blend_tree(float ifps, float speed, const StateMachine &state_machine, ObjectMeshSkinnedPtr &skinned)
{
	if (animations_data.empty())
		return;

	if (animations_data.size() == 1)
	{
		update_animation(ifps, speed, state_machine, skinned);
		return;
	}

	// check end of playback
	if (isEnd())
	{
		root_motion_delta = mat4_identity;
		return;
	}

	// next frame
	frame += ifps * speed;
	frame = clamp(frame, 0.0f, itof(getNumFrames() - 1));

	const int last_index = getNumFrames() - 1;
	if (ftoi(frame) == last_index)
	{
		// get delta from current position to last frame
		setFrame(itof(last_index), state_machine, skinned);
		for (int i = 0; i < animations_data.size(); i++)
			animations_data[i].last_delta = animations_data[i].root_motion_delta;

		if (isLoop())
		{
			frame -= itof(last_index);
			resetForFrame(state_machine, skinned, 0.0f);
		} else
		{
			frame = itof(last_index);
		}
	} else
	{
		for (int i = 0; i < animations_data.size(); i++)
			animations_data[i].last_delta = mat4_identity;
	}

	// final result
	setFrame(frame, state_machine, skinned);

	if (state_machine.isUseRootMotion())
	{
		root_motion_delta = mat4_identity;

		float total_weight = 0.0f;
		for (int i = 0; i < animations_data.size(); i++)
		{
			if (animations_data[i].weight < Consts::EPS)
				continue;

			total_weight += animations_data[i].weight;
			root_motion_delta = lerp(root_motion_delta, animations_data[i].root_motion_delta, animations_data[i].weight / total_weight);
		}
	}
}

void State::update_weights_1d()
{
	for (int i = 0; i < animations_data.size(); i++)
		animations_data[i].weight = 0.0f;

	if (animations_data.size() != 0)
	{
		if (blend_point.x < animations_data.first().anchor.x)
		{
			animations_data.first().weight = 1.0f;
			return;
		}

		if (animations_data.last().anchor.x < blend_point.x)
		{
			animations_data.last().weight = 1.0f;
			return;
		}
	}

	for (int i = 0; i < animations_data.size() - 1; i++)
	{
		AnimData &a = animations_data[i];
		AnimData &b = animations_data[i + 1];

		if (a.anchor.x <= blend_point.x && blend_point.x <= b.anchor.x)
		{
			a.weight = (b.anchor.x - blend_point.x) / (b.anchor.x - a.anchor.x);
			b.weight = 1.0f - a.weight;
			break;
		}
	}
}

void State::update_weights_2d()
{
	blend_point = clamp(blend_point, min_anchors_value, max_anchors_value);

	// gradient band interpolation
	float total_weight = 0.0f;
	for (int i = 0; i < animations_data.size(); i++)
	{
		const vec2 projected_vector = blend_point - animations_data[i].anchor;

		float min_weight = Consts::INF;
		for (int j = 0; j < animations_data.size(); j++)
		{
			if (j == i)
				continue;

			vec2 projection_direction = animations_data[j].anchor - animations_data[i].anchor;
			float weight = 1.0f - dot(projected_vector, projection_direction) / projection_direction.length2();
			min_weight = min(min_weight, max(weight, 0.0f));
		}

		animations_data[i].weight = min_weight;
		total_weight += min_weight;
	}

	for (int i = 0; i < animations_data.size(); i++)
		animations_data[i].weight = animations_data[i].weight / total_weight;
}

///////////////////////////////////////////////////////////////////////////////////
// State Machine
///////////////////////////////////////////////////////////////////////////////////

Map<String, Animation *> StateMachine::animations;
int StateMachine::state_machines_count = 0;
Mutex StateMachine::anim_mutex;

StateMachine::StateMachine()
{
	ScopedLock lock(anim_mutex);
	state_machines_count++;
}

StateMachine::~StateMachine()
{
	ScopedLock lock(anim_mutex);
	state_machines_count--;

	if (state_machines_count == 0)
	{
		for (const auto &pair : animations)
			delete pair.data;
		animations.clear();
	}
}

void StateMachine::initFromProperty(ObjectMeshSkinnedPtr &object, const PropertyPtr &prop, AXIS in_axis_forward)
{
	connections.disconnectAll();

	axis_forward = in_axis_forward;

	skinned = object;
	skinned->stop();
	skinned->setInterpolationAccuracy(ObjectMeshSkinned::INTERPOLATION_ACCURACY_LOW);

	skinned->setNumLayers(NUM_LAYERS);

	skinned->getEventUpdate().connect(connections, this, &StateMachine::update);
	engine_update_frame = 0;

	PropertyParameterPtr root = prop->getParameterPtr();

	animations_frame_rate = root->getChild("animations_frame_rate")->getValueInt();
	playback_speed = root->getChild("playback_speed")->getValueFloat();

	// features
	use_root_motion = root->getChild("use_root_motion")->getValueToggle();
	use_smooth_transitions = root->getChild("use_smooth_transitions")->getValueToggle();
	use_foot_matching = root->getChild("use_foot_matching")->getValueToggle();

	// bones
	root_bone_name = root->getChild("root_bone_name")->getValueString();
	left_foot_bone_name = root->getChild("left_foot_bone_name")->getValueString();
	right_foot_bone_name = root->getChild("right_foot_bone_name")->getValueString();

	root_bone = skinned->findBone(root_bone_name);
	left_foot_bone = skinned->findBone(left_foot_bone_name);
	right_foot_bone = skinned->findBone(right_foot_bone_name);

	// state variables
	float_variables.clear();
	float_indices.clear();
	flag_variables.clear();
	flag_indices.clear();

	add_float_variable("state_time", 0.0f);

	int state_variables_index = root->findChild("state_variables");
	if (state_variables_index != -1)
	{
		PropertyParameterPtr variables_param = root->getChild(state_variables_index);
		int num = variables_param->getNumChildren();
		for (int i = 0; i < num; i++)
		{
			PropertyParameterPtr variable_param = variables_param->getChild(i);
			int type = variable_param->getChild("type")->getValueInt();
			const char *name = variable_param->getChild("name")->getValueString();
			float default_value = variable_param->getChild("default_value")->getValueFloat();

			if (type == 0)
				add_float_variable(name, default_value);
			else if (type == 1)
				add_flag_variable(name, ftoi(default_value));
		}
	}

	// states
	states.clear();

	int states_index = root->findChild("states");
	if (states_index != -1)
	{
		PropertyParameterPtr states_param = root->getChild(states_index);
		int num = states_param->getNumChildren();
		for (int i = 0; i < num; i++)
		{
			PropertyParameterPtr state_param = states_param->getChild(i);

			// general
			const char *name = state_param->getChild("name")->getValueString();
			int state_type = state_param->getChild("state_type")->getValueInt();
			float min_playback_time = state_param->getChild("min_playback_time")->getValueFloat();
			bool is_loop = state_param->getChild("is_loop")->getValueToggle();
			bool is_use_random_animation = state_param->getChild("use_random_animation")->getValueToggle();
			float foot_matching_begin = state_param->getChild("foot_matching_begin")->getValueFloat();
			float foot_matching_end = state_param->getChild("foot_matching_end")->getValueFloat();
			bool foot_matching_override = state_param->getChild("foot_matching_override")->getValueToggle();
			const char *x_blend_variable = state_param->getChild("x_blend_variable")->getValueString();
			const char *y_blend_variable = state_param->getChild("y_blend_variable")->getValueString();

			State &state = add_state(name, state_type);
			state.setMinPlaybackTime(min_playback_time);
			state.setLoop(is_loop);
			state.setUseRandomAnimation(is_use_random_animation);
			state.setFootMatching(foot_matching_begin, foot_matching_end, foot_matching_override);

			int var_type = getVariableType(x_blend_variable);
			int var_id = getVariableID(var_type, x_blend_variable);
			state.setBlendXVariable(var_type, var_id);

			var_type = getVariableType(y_blend_variable);
			var_id = getVariableID(var_type, y_blend_variable);
			state.setBlendYVariable(var_type, var_id);

			// animations
			PropertyParameterPtr animations_param = state_param->getChild("animations");
			int num_animations = animations_param->getNumChildren();
			for (int j = 0; j < num_animations; j++)
			{
				PropertyParameterPtr anim_param = animations_param->getChild(j);
				const char *anim_path = anim_param->getChild("path")->getValueFile();
				vec2 anim_anchor = anim_param->getChild("anchor")->getValueVec2();

				Animation *anim = load_animation(anim_path, root_bone, left_foot_bone, right_foot_bone);

				switch (state.getType())
				{
					case State::TYPE_ANIMATION: state.addAnimation(anim, skinned);                       break;
					case State::TYPE_BLEND_1D:  state.addBlend1DAnimation(anim, anim_anchor.x, skinned); break;
					case State::TYPE_BLEND_2D:  state.addBlend2DAnimation(anim, anim_anchor, skinned);   break;
				}
			}

			// transitions
			PropertyParameterPtr transitions_param = state_param->getChild("transitions");
			int num_transitions = transitions_param->getNumChildren();
			for (int j = 0; j < num_transitions; j++)
			{
				PropertyParameterPtr transition_param = transitions_param->getChild(j);
				const char *next_state = transition_param->getChild("next_state")->getValueString();
				const char *condition = transition_param->getChild("condition")->getValueString();
				float half_life = transition_param->getChild("half_life")->getValueFloat();
				state.addTransition(*this, next_state, condition, half_life);
			}
		}
	}

	// initial state
	current_state_name = "";
	current_state_hash = 0;

	int initial_state_index = root->findChild("init_state_name");
	if (initial_state_index != -1)
	{
		const char *state_name = root->getChild(initial_state_index)->getValueString();
		setCurrentStateName(state_name, true);
	}

	interpolator.reset(skinned);

	root_motion_speed = 0.0f;
}

void StateMachine::update(float ifps)
{
	engine_update_frame = Engine::get()->getFrame();

	if (is_update_enabled == false)
		return;

	if (skinned.isValid() == false)
		return;

	auto res = states.findFast(current_state_name);
	if (res == nullptr)
		return;

	if (isUseSmoothTransitions())
		interpolator.update(ifps);

	State &state = res->data;
	state.update(ifps, animations_frame_rate * playback_speed, *this, &interpolator, skinned);
	setFloatVariable(0, state.getStateTime());

	// apply state frame
	if (isUseSmoothTransitions() && res != nullptr && res->data.isEnd() == false)
		interpolator.applyCurrentOffsets(skinned);

	if (isUseRootMotion())
	{
		const mat4 bone_t = skinned->getLayerBoneTransform(LAYER_RESULT, getRootBone());
		const mat4 proj_bone_t = getTransformProj(bone_t, axis_forward);
		skinned->setLayerBoneTransform(LAYER_RESULT, getRootBone(), inverse(proj_bone_t) * bone_t);
	}

	float transition_duration = 0.0f;

	const char *next_state = state.checkTransition(*this, transition_duration);
	if (next_state != nullptr)
	{
		res = states.findFast(next_state);
		if (res != nullptr)
		{
			State &next_state = res->data;
			init_transition(state, next_state, transition_duration);
		}
	}

	root_motion_speed = getRootMotionDelta().getTranslate().length() / max(Consts::EPS, ifps);
}

void StateMachine::setCurrentStateName(const char *name, bool is_immediate_transition, float transition_duration /*= 0.2f*/)
{
	if (states.contains(name))
	{
		if (is_immediate_transition)
		{
			current_state_name = name;
			states[current_state_name].init(*this, skinned);
			current_state_hash = states.findFast(current_state_name)->hash;
			interpolator.clearOffsets();
		} else
		{
			State &state = states[current_state_name];
			State &next_state = states[name];
			init_transition(state, next_state, transition_duration);
		}

	} else
	{
		current_state_name = "";
		current_state_hash = 0;
	}
}

unsigned int StateMachine::getStateHash(const char *state_name) const
{
	auto res = states.findFast(state_name);
	if (res != nullptr)
		return res->hash;

	return 0;
}

int StateMachine::getCurrentStateNumFrames() const
{
	auto res = states.findFast(current_state_name);
	if (res != nullptr)
		return res->data.getNumFrames();

	return 0;
}

float StateMachine::getCurrentStateFrame() const
{
	auto res = states.findFast(current_state_name);
	if (res != nullptr)
		return res->data.getFrame();

	return 0.0f;
}

void StateMachine::setFloatVariable(const char *name, float value)
{
	auto res = float_indices.findFast(name);
	if (res != nullptr)
	{
		int index = res->data;
		float_variables[index] = value;
	}
}

float StateMachine::getFloatVariable(const char *name) const
{
	auto res = float_indices.findFast(name);
	if (res != nullptr)
	{
		int index = res->data;
		return float_variables[index];
	}

	return 0.0f;
}

int StateMachine::getFloatVariableID(const char *name) const
{
	auto res = float_indices.findFast(name);
	if (res != nullptr)
		return res->data;

	return -1;
}

void StateMachine::setFlagVariable(const char *name, int value)
{
	auto res = flag_indices.findFast(name);
	if (res != nullptr)
	{
		int index = res->data;
		flag_variables[index] = clamp(value, 0, 1);
	}
}

int StateMachine::getFlagVariable(const char *name) const
{
	auto res = flag_indices.findFast(name);
	if (res != nullptr)
	{
		int index = res->data;
		return flag_variables[index];
	}

	return 0;
}

int StateMachine::getFlagVariableID(const char *name) const
{
	auto res = flag_indices.findFast(name);
	if (res != nullptr)
		return res->data;

	return -1;
}

int StateMachine::getVariableType(const char *name) const
{
	if (float_indices.contains(name))
		return VAR_TYPE_FLOAT;

	if (flag_indices.contains(name))
		return VAR_TYPE_FLAG;

	return VAR_TYPE_NONE;
}

int StateMachine::getVariableID(int type, const char *name) const
{
	switch (type)
	{
		case VAR_TYPE_FLOAT:
			if (float_indices.contains(name))
				return float_indices[name];
			break;
		case VAR_TYPE_FLAG:
			if (flag_indices.contains(name))
				return flag_indices[name];
			break;
	}

	return -1;
}

float StateMachine::getStateVariable(int type, int id) const
{
	switch (type)
	{
		case VAR_TYPE_FLOAT: return float_variables[id];
		case VAR_TYPE_FLAG:  return itof(flag_variables[id]);
	}

	return 0.0f;
}

mat4 StateMachine::getRootMotionDelta() const
{
	const long long frame = Engine::get()->getFrame();
	if (frame != engine_update_frame && frame != (engine_update_frame + 1))
		return mat4_identity;

	if (states.contains(current_state_name))
	{
		const State &state = states[current_state_name];
		return state.getRootMotionDelta();
	}

	return mat4_identity;
}

void StateMachine::init_transition(State &current_state, State &next_state, float duration)
{
	current_state.clearTime();

	// src
	if (isUseSmoothTransitions())
		interpolator.initSrcValues(skinned);

	current_state_name = next_state.getName();
	current_state_hash = states.findFast(current_state_name)->hash;
	next_state.init(*this, skinned);

	if (isUseFootMatching())
	{
		int best_anim_index = 0;
		int best_frame = 0;
		const vec3 &src_left_foot_pos = current_state.getLeftFootPosition();
		const vec3 &src_right_foot_pos = current_state.getRightFootPosition();
		next_state.findBestFootAnimation(best_anim_index, best_frame, *this, src_left_foot_pos, src_right_foot_pos);
		next_state.resetForFrame(*this, skinned, itof(best_frame), best_anim_index);
	}

	// dst
	if (isUseSmoothTransitions())
	{
		interpolator.initDstValues(skinned);
		interpolator.calculateOffsets(duration);
	}

	next_state.clearTime();

	if (isUseSmoothTransitions())
		interpolator.applyCurrentOffsets(skinned);

	if (isUseRootMotion())
	{
		const mat4 bone_t = skinned->getLayerBoneTransform(LAYER_RESULT, getRootBone());
		const mat4 proj_bone_t = getTransformProj(bone_t, axis_forward);
		skinned->setLayerBoneTransform(LAYER_RESULT, getRootBone(), inverse(proj_bone_t) * bone_t);
	}
}

State &StateMachine::add_state(const char *name, int type)
{
	if (states.contains(name))
	{
		if (states[name].getType() != type)
		{
			states.remove(name);
			states.insert(name, State(name, type, axis_forward));
		}
	} else
	{
		states.insert(name, State(name, type, axis_forward));
	}

	states[name].init(*this, skinned);
	return states[name];
}

void StateMachine::add_float_variable(const char *name, float value)
{
	if (float_indices.contains(name))
	{
		int index = float_indices[name];
		float_variables[index] = value;
	} else
	{
		float_variables.append(value);
		float_indices.insert(name, float_variables.size() - 1);
	}
}

void StateMachine::add_flag_variable(const char *name, int value)
{
	value = clamp(value, 0, 1);

	if (flag_indices.contains(name))
	{
		int index = flag_indices[name];
		flag_variables[index] = value;
	} else
	{
		flag_variables.append(value);
		flag_indices.insert(name, flag_variables.size() - 1);
	}
}

Animation *StateMachine::load_animation(const char *path, int root_bone_index, int left_foot_bone_index, int right_foot_bone_index)
{
	ScopedLock lock(anim_mutex);

	if (animations.contains(path))
		return animations[path];

	Animation *anim = new Animation();
	anim->path = path;

	MeshAnimationPtr mesh = MeshAnimation::create();
	if (mesh->load(path) != 0)
	{
		anim->num_frames = mesh->getNumFrames();
		anim->left_foot_positions.resize(anim->num_frames);
		anim->right_foot_positions.resize(anim->num_frames);

		const int num_mesh_bones = mesh->getNumBones();

		Vector<short> anim_bones;
		mesh->getAnimationBones(anim_bones);

		auto get_bone_transform = [&mesh](int bone, const Vector<mat4> &bone_transforms)
		{
			mat4 t{bone_transforms[bone]};

			int parent = mesh->getBoneParent(bone);
			while (parent != -1)
			{
				t = bone_transforms[parent] * t;
				parent = mesh->getBoneParent(parent);
			}

			return t;
		};

		Vector<mat4> transforms;
		for (int i = 0; i < anim->num_frames; i++)
		{
			mesh->getFrame(i, transforms);

			Vector<mat4> mesh_bone_transforms;
			mesh_bone_transforms.resize(num_mesh_bones);
			for (int j = 0; j < num_mesh_bones; j++)
			{
				int anim_bone_index = anim_bones.findIndex(j);
				if (anim_bone_index == -1)
					mesh_bone_transforms[j] = mesh->getBoneTransform(j);
				else
					mesh_bone_transforms[j] = transforms[anim_bone_index];
			}

			mat4 root_it = inverse(get_bone_transform(root_bone_index, mesh_bone_transforms));
			mat4 left_foot_t = get_bone_transform(left_foot_bone_index, mesh_bone_transforms);
			mat4 right_foot_t = get_bone_transform(right_foot_bone_index, mesh_bone_transforms);

			anim->left_foot_positions[i] = (root_it * left_foot_t).getTranslate();
			anim->right_foot_positions[i] = (root_it * right_foot_t).getTranslate();
		}
	}

	animations.append(path, anim);
	return anim;
}





}
