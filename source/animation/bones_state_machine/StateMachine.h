#pragma once
#include <UnigineObjects.h>
#include <UnigineVector.h>

namespace AnimationSystem
{

enum LAYERS
{
	LAYER_RESULT = 0,
	LAYER_AUXILIARY,
	NUM_LAYERS,
};

enum VAR_TYPE
{
	VAR_TYPE_NONE = 0,
	VAR_TYPE_FLOAT,
	VAR_TYPE_FLAG,
};

///////////////////////////////////////////////////////////////////////////////////
// Animation
///////////////////////////////////////////////////////////////////////////////////

struct Animation
{
	Unigine::String path;
	int num_frames{0};

	Unigine::Vector<Unigine::Math::vec3> left_foot_positions;
	Unigine::Vector<Unigine::Math::vec3> right_foot_positions;
};

///////////////////////////////////////////////////////////////////////////////////
// Interpolator
///////////////////////////////////////////////////////////////////////////////////

// https://theorangeduck.com/page/spring-roll-call#critical
struct Interpolator
{
	void initSrcValues(const Unigine::ObjectMeshSkinnedPtr &skinned);
	void initDstValues(const Unigine::ObjectMeshSkinnedPtr &skinned);

	void calculateOffsets(float in_half_life);
	void update(float ifps);

	void reset(const Unigine::ObjectMeshSkinnedPtr &skinned);
	void applyCurrentOffsets(Unigine::ObjectMeshSkinnedPtr &skinned);
	void clearOffsets();

	Unigine::Vector<Unigine::Math::vec3> src_positions;
	Unigine::Vector<Unigine::Math::quat> src_rotations;

	Unigine::Vector<Unigine::Math::vec3> dst_positions;
	Unigine::Vector<Unigine::Math::quat> dst_rotations;

	Unigine::Vector<Unigine::Math::vec3> offset_positions;
	Unigine::Vector<Unigine::Math::quat> offset_rotations;
	Unigine::Vector<Unigine::Math::vec3> offset_linear_velocities;
	Unigine::Vector<Unigine::Math::vec3> offset_angular_velocities;

	float half_life{0.2f};
	float active_time{0.0f};
};

///////////////////////////////////////////////////////////////////////////////////
// Transition
///////////////////////////////////////////////////////////////////////////////////

class State;
class StateMachine;

struct Transition
{
	enum FUNC
	{
		FUNC_END = 0,	   // last frame
		FUNC_GREATER,	   // var > const
		FUNC_LESS,		   // var < const
		FUNC_EQUAL,		   // var = const
		FUNC_NOT_EQUAL,	   // var != const
		FUNC_IN_RANGE,	   // var in const_0;const_1
		FUNC_OUT_RANGE,	   // var out const_0;const_1
	};

	struct Condition
	{
		int var_type{VAR_TYPE_NONE};
		int var_id{-1};

		int func{FUNC_END};
		float const_0{0.0f};
		float const_1{0.0f};
	};

	Unigine::StringStack<> next_state;

	// end
	// speed > 0.2
	// turn out -0.1;0.1
	// speed > 0.2 AND turn in -0.1;0.1
	// need_stop == 0.0 AND speed > 0.2 AND turn out -0.2;0.2
	// etc.
	Unigine::Vector<Condition> conditions;
	float half_life{0.2f};

	void parseConditions(const StateMachine &state_machine, const char *conditions_str);
	bool check(const StateMachine &state_machine, const State *state) const;
};

///////////////////////////////////////////////////////////////////////////////////
// State
///////////////////////////////////////////////////////////////////////////////////

class State
{
public:
	enum TYPE
	{
		TYPE_ANIMATION = 0,
		TYPE_BLEND_1D,
		TYPE_BLEND_2D
	};

	State() = default;
	State(const char *name, int type, Unigine::Math::AXIS axis_forward = Unigine::Math::AXIS_Z);
	~State() = default;

	// general
	int getType() const { return type; }
	const char *getName() const { return name; }

	void setMinPlaybackTime(float time) { min_playback_time = time; }
	float getStateTime() const { return state_time; }
	void clearTime() { state_time = 0.0f; }

	void addTransition(const StateMachine &state_machine, const char *next_state, const char *conditions, float half_life);
	const char *checkTransition(const StateMachine &state_machine, float &out_transition_half_life);

	void init(const StateMachine &state_machine, Unigine::ObjectMeshSkinnedPtr &skinned);
	void resetForFrame(const StateMachine &state_machine, Unigine::ObjectMeshSkinnedPtr &skinned, float in_frame, int anim_index = -1);
	void findBestFootAnimation(int &out_anim_index, int &out_frame, const StateMachine &state_machine, const Unigine::Math::vec3 &left_foot_pos, const Unigine::Math::vec3 &right_foot_pos);
	void update(float ifps, float speed, const StateMachine &state_machine, Interpolator *interpolator, Unigine::ObjectMeshSkinnedPtr &skinned);

	void setFrame(float in_frame, const StateMachine &state_machine, Unigine::ObjectMeshSkinnedPtr &skinned);
	float getFrame() const { return frame; }
	float getNormalizedFrame() const { return getFrame() / Unigine::Math::itof(getNumFrames() - 1); }
	int getNumFrames() const { return num_frames; }

	void setUseRandomAnimation(bool enabled) { is_use_random_animation = enabled; }
	bool isUseRandomAnimation() const { return type == TYPE_ANIMATION && is_use_random_animation; }

	void setLoop(bool loop) { is_loop = loop; }
	bool isLoop() const { return is_loop; }

	bool isEnd() const { return isLoop() == false && frame == (getNumFrames() - 1); }

	void setFootMatching(float begin, float end, bool is_override);

	const Unigine::Math::mat4 &getRootMotionDelta() const { return root_motion_delta; }

	// animation
	void addAnimation(Animation *animation, const Unigine::ObjectMeshSkinnedPtr &skinned);

	// blend 1d
	void addBlend1DAnimation(Animation *animation, float anchor, const Unigine::ObjectMeshSkinnedPtr &skinned);
	void setBlendXVariable(int type, int id)
	{
		blend_x_var_type = type;
		blend_x_var_id = id;
	}

	// blend 2d
	void addBlend2DAnimation(Animation *animation, const Unigine::Math::vec2 &anchor, const Unigine::ObjectMeshSkinnedPtr &skinned);
	void setBlendYVariable(int type, int id)
	{
		blend_y_var_type = type;
		blend_y_var_id = id;
	}

	const Unigine::Math::vec3 &getLeftFootPosition() const { return left_foot_pos; }
	const Unigine::Math::vec3 &getRightFootPosition() const { return right_foot_pos; }

private:
	void update_animation(float ifps, float speed, const StateMachine &state_machine, Unigine::ObjectMeshSkinnedPtr &skinned);
	void update_blend_tree(float ifps, float speed, const StateMachine &state_machine, Unigine::ObjectMeshSkinnedPtr &skinned);

	void update_weights_1d();
	void update_weights_2d();

private:
	Unigine::StringStack<> name;
	int type{TYPE_ANIMATION};
	Unigine::Math::AXIS axis_forward = Unigine::Math::AXIS_Z;
	int num_frames{0};
	float frame{0.0f};
	float min_playback_time{0.0f};
	float state_time{0.0f};

	struct AnimData
	{
		Animation *animation{nullptr};
		long long anim_id{0};
		Unigine::Math::vec2 anchor{Unigine::Math::vec2_zero};
		float weight{0.0f};

		Unigine::Math::mat4 last_delta{Unigine::Math::mat4_identity};

		Unigine::Math::mat4 prev_root_t{Unigine::Math::mat4_identity};
		Unigine::Math::mat4 root_motion_delta{Unigine::Math::mat4_identity};
	};

	Unigine::Vector<AnimData> animations_data;

	Unigine::Vector<Transition> transitions;
	float foot_matching_begin{0.0f};
	float foot_matching_end{1.0f};
	bool foot_matching_override{true};

	bool is_use_random_animation{false};
	bool is_loop{false};
	int current_anim_index{0};

	int blend_x_var_type{VAR_TYPE_NONE};
	int blend_x_var_id{-1};

	int blend_y_var_type{VAR_TYPE_NONE};
	int blend_y_var_id{-1};

	Unigine::Math::vec2 min_anchors_value{Unigine::Math::vec2_zero};
	Unigine::Math::vec2 max_anchors_value{Unigine::Math::vec2_zero};
	Unigine::Math::vec2 blend_point{Unigine::Math::Consts::INF};

	Unigine::Math::mat4 root_motion_delta{Unigine::Math::mat4_identity};

	Unigine::Math::vec3 left_foot_pos;
	Unigine::Math::vec3 right_foot_pos;
};

///////////////////////////////////////////////////////////////////////////////////
// State Machine
///////////////////////////////////////////////////////////////////////////////////

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void initFromProperty(Unigine::ObjectMeshSkinnedPtr &object, const Unigine::PropertyPtr &prop, Unigine::Math::AXIS axis_forward = Unigine::Math::AXIS_Z);
	void update(float ifps);

	void setUpdateEnabled(bool enabled) { is_update_enabled = enabled; }
	bool isUpdateEnabled() const { return is_update_enabled; }

	void setAnimationsFrameRate(int frame_rate) { animations_frame_rate = frame_rate; }
	int getAnimationsFrameRate() const { return animations_frame_rate; }

	void setPlaybackSpeed(float speed) { playback_speed = speed; }
	float getPlaybackSpeed() const { return playback_speed; }

	// features
	void setUseRootMotion(bool enabled) { use_root_motion = enabled; }
	bool isUseRootMotion() const { return use_root_motion; }

	void setUseSmoothTransitions(bool enabled) { use_smooth_transitions = enabled; }
	bool isUseSmoothTransitions() const { return use_smooth_transitions; }

	void setUseFootMatching(bool enabled) { use_smooth_transitions = enabled; }
	bool isUseFootMatching() const { return use_foot_matching; }

	// bones
	const char *getRootBoneName() const { return root_bone_name; }
	const char *getLeftFootBoneName() const { return left_foot_bone_name; }
	const char *getRightFootBoneName() const { return right_foot_bone_name; }

	int getRootBone() const { return root_bone; }
	int getLeftFootBone() const { return left_foot_bone; }
	int getRightFootBone() const { return right_foot_bone; }

	// states
	void getStates(Unigine::Vector<Unigine::StringStack<>> &names) const { states.getKeys(names); }

	void setCurrentStateName(const char *name, bool is_immediate_transition, float transition_duration = 0.2f);
	const char *getCurrentStateName() const { return current_state_name; }
	unsigned int getCurrentStateHash() const { return current_state_hash; }

	unsigned int getStateHash(const char *state_name) const;

	int getCurrentStateNumFrames() const;
	float getCurrentStateFrame() const;

	// float variables
	void setFloatVariable(const char *name, float value);
	float getFloatVariable(const char *name) const;

	void setFloatVariable(int id, float value) { float_variables[id] = value; }
	float getFloatVariable(int id) const { return float_variables[id]; }

	bool containsFloatVariable(const char *name) const { return float_indices.contains(name); }
	int getFloatVariableID(const char *name) const;
	void getFloatVariables(Unigine::Vector<Unigine::StringStack<>> &names) const { float_indices.getKeys(names); }

	// flag variables
	void setFlagVariable(const char *name, int value);
	int getFlagVariable(const char *name) const;

	void setFlagVariable(int id, int value) { flag_variables[id] = Unigine::Math::clamp(value, 0, 1); }
	int getFlagVariable(int id) const { return flag_variables[id]; }

	bool containsFlagVariable(const char *name) const { return flag_indices.contains(name); }
	int getFlagVariableID(const char *name) const;
	void getFlagVariables(Unigine::Vector<Unigine::StringStack<>> &names) const { flag_indices.getKeys(names); }

	// variables
	int getVariableType(const char *name) const;
	int getVariableID(int type, const char *name) const;

	bool containsVariable(const char *name) const
	{
		return containsFloatVariable(name) || containsFlagVariable(name);
	}

	float getStateVariable(int type, int id) const;

	Unigine::Math::mat4 getRootMotionDelta() const;
	float getRootMotionSpeed() const { return root_motion_speed; }

private:
	// transitions
	void init_transition(State &current_state, State &next_state, float duration);

	// states
	State &add_state(const char *name, int type);

	// variables
	void add_float_variable(const char *name, float value);
	void add_flag_variable(const char *name, int value);

	static Animation *load_animation(const char *path, int root_bone_index, int left_foot_bone_index, int right_foot_bone_index);

private:
	Unigine::ObjectMeshSkinnedPtr skinned;
	Unigine::EventConnections connections;
	bool is_update_enabled{true};
	long long engine_update_frame{0};
	Unigine::Math::AXIS axis_forward = Unigine::Math::AXIS_Z;

	int animations_frame_rate{30};
	float playback_speed{1.0f};

	// features
	bool use_root_motion{true};
	bool use_smooth_transitions{true};
	bool use_foot_matching{true};

	// bones
	Unigine::StringStack<> root_bone_name;
	Unigine::StringStack<> left_foot_bone_name;
	Unigine::StringStack<> right_foot_bone_name;
	int root_bone{-1};
	int left_foot_bone{-1};
	int right_foot_bone{-1};

	// variables
	Unigine::Vector<float> float_variables;
	Unigine::HashMap<Unigine::StringStack<>, int> float_indices;

	Unigine::Vector<int> flag_variables;
	Unigine::HashMap<Unigine::StringStack<>, int> flag_indices;

	// states
	Unigine::HashMap<Unigine::StringStack<>, State> states;
	Unigine::StringStack<> current_state_name;
	unsigned int current_state_hash{0};

	Interpolator interpolator;

	float root_motion_speed{0.0f};

	// animations
	static Unigine::Map<Unigine::String, Animation *> animations;
	static int state_machines_count;
	static Unigine::Mutex anim_mutex;
};

}
