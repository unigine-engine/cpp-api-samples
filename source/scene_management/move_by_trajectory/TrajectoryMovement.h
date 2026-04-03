// Trajectory-based movement implementations for smooth path following.
// Provides linear interpolation, Catmull-Rom spline, and .path file playback.
// All variants loop through waypoints with configurable velocity.

#pragma once

#include "UnigineComponentSystem.h"
#include "UnigineVector.h"

#include <UniginePath.h>
#include <UnigineWorlds.h>

// Moves node along waypoints using linear position and slerp rotation interpolation.
class SimpleTrajectoryMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleTrajectoryMovement, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Parent node containing child waypoints
	PROP_PARAM(Node, path_node);
	// Movement speed in units per second
	PROP_PARAM(Float, velocity, 10.0f);
	// Enables path visualization in debug mode
	PROP_PARAM(Toggle, debug);

private:
	void init();
	void update();
	void visualize_path();
	// Advances interpolation time based on velocity and segment length
	void update_time();

	// Cached waypoint positions and rotations
	Unigine::Vector<Unigine::Math::Vec3> points_pos;
	Unigine::Vector<Unigine::Math::quat> points_rot;
	// Previous segment endpoint for interpolation
	Unigine::Math::Vec3 prev_point;
	Unigine::Math::quat prev_rot;
	// Current target waypoint index
	int points_index = 0;
	// Interpolation progress (0 to 1) within current segment
	float time = 0.0f;
};

// Moves node along waypoints using Catmull-Rom spline with SQUAD rotation.
class SplineTrajectoryMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SplineTrajectoryMovement, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Parent node containing child waypoints
	PROP_PARAM(Node, pathNode);
	// Movement speed in units per second
	PROP_PARAM(Float, velocity, 10.0f);
	// Spline subdivision count for length calculation
	PROP_PARAM(Int, quality, 25);
	// Enables path visualization in debug mode
	PROP_PARAM(Toggle, debug);

private:
	void init();
	void update();
	void visualize_path();
	// Advances time using precomputed segment lengths for constant velocity
	void update_time(float speed);

	// Returns four control points for current spline segment
	Unigine::VectorStack<Unigine::Math::Vec3, 4> get_current_points() const;
	// Returns four control rotations for current spline segment
	Unigine::VectorStack<Unigine::Math::quat, 4> get_current_quats() const;

	// Precomputed segment lengths for speed normalization
	Unigine::Vector<Unigine::Vector<float>> lengths;
	Unigine::Vector<Unigine::Math::Vec3> points_pos;
	Unigine::Vector<Unigine::Math::quat> points_rot;
	int points_index = 0;
	float time = 0.0f;
};

// Converts waypoints to UNIGINE .path file format using spline interpolation.
class PathTrajectorySaver : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PathTrajectorySaver, ComponentBase);
	COMPONENT_INIT(init, -1);

	// Output .path file path
	PROP_PARAM(File, path_file);
	// Parent node containing child waypoints
	PROP_PARAM(Node, path_node);
	// Spline subdivision count per segment
	PROP_PARAM(Int, quality, 25);
	// Saves path file automatically on initialization
	PROP_PARAM(Toggle, autosave, 1);

private:
	void init();
	// Generates .path file from waypoints with spline interpolation
	void save();
};

// Plays back pre-saved .path files using WorldTransformPath for smooth movement.
class SavedPathTrajectory : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SavedPathTrajectory, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Path to .path file containing trajectory data
	PROP_PARAM(File, trajectory_file_path);
	// Playback speed multiplier
	PROP_PARAM(Float, velocity, 10.0f);
	// Enables path visualization in debug mode
	PROP_PARAM(Toggle, debug);

private:
	void init();
	void update();
	void visualize_path();

	// Built-in path playback handler
	Unigine::WorldTransformPathPtr transform_path;
};
