#pragma once

#include "UnigineComponentSystem.h"
#include "UnigineVector.h"

#include <UniginePath.h>
#include <UnigineWorlds.h>

class SimpleTrajectoryMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleTrajectoryMovement, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, path_node);
	PROP_PARAM(Float, velocity, 10.0f);
	PROP_PARAM(Toggle, debug);

private:
	void init();
	void update();

	void visualize_path();

	void update_time();

	Unigine::Vector<Unigine::Math::Vec3> points_pos;
	Unigine::Vector<Unigine::Math::quat> points_rot;
	Unigine::Math::Vec3 prev_point;
	Unigine::Math::quat prev_rot;
	int points_index = 0;
	float time = 0.0f;
};

class SplineTrajectoryMovement : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SplineTrajectoryMovement, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(Node, pathNode);
	PROP_PARAM(Float, velocity, 10.0f);
	PROP_PARAM(Int, quality, 25);
	PROP_PARAM(Toggle, debug);

private:
	void init();
	void update();

	void visualize_path();

	void update_time(float speed);

	Unigine::VectorStack<Unigine::Math::Vec3, 4> get_current_points() const;
	Unigine::VectorStack<Unigine::Math::quat, 4> get_current_quats() const;

	Unigine::Vector<Unigine::Vector<float>> lengths;
	Unigine::Vector<Unigine::Math::Vec3> points_pos;
	Unigine::Vector<Unigine::Math::quat> points_rot;
	int points_index = 0;
	float time = 0.0f;
};

class PathTrajectorySaver : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PathTrajectorySaver, ComponentBase);
	COMPONENT_INIT(init, -1);

	PROP_PARAM(File, path_file);
	PROP_PARAM(Node, path_node);
	PROP_PARAM(Int, quality, 25);
	PROP_PARAM(Toggle, autosave, 1);

private:
	void init();
	void save();
};

class SavedPathTrajectory : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SavedPathTrajectory, ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(File, trajectory_file_path);
	PROP_PARAM(Float, velocity, 10.0f);
	PROP_PARAM(Toggle, debug);

private:
	void init();
	void update();

	void visualize_path();

	Unigine::WorldTransformPathPtr transform_path;
};
