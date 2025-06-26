#include "TrajectoryMovement.h"

#include "../../utils/Math.h"
#include "UnigineGame.h"
#include "UnigineVisualizer.h"

#include <UniginePrimitives.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(SimpleTrajectoryMovement);
REGISTER_COMPONENT(SplineTrajectoryMovement);
REGISTER_COMPONENT(PathTrajectorySaver);
REGISTER_COMPONENT(SavedPathTrajectory);

void SimpleTrajectoryMovement::init()
{
	UNIGINE_ASSERT(!path_node.nullCheck());

	// save positions and rotations
	int num_childs = path_node->getNumChildren();
	for (int i = 0; i < num_childs; i++)
	{
		NodePtr nc = path_node->getChild(i);
		points_pos.emplace_back(nc->getWorldPosition());
		points_rot.emplace_back(nc->getWorldRotation());
	}
	prev_point = node->getWorldPosition();
}

void SimpleTrajectoryMovement::update()
{
	// update time
	update_time();

	// set position and rotation
	node->setWorldPosition(lerp(prev_point, points_pos[points_index], time));
	node->setWorldRotation(slerp(prev_rot, points_rot[points_index], time));

	// visualize path if enabled
	if (debug.get())
		visualize_path();
}

void SimpleTrajectoryMovement::visualize_path()
{
	for (int i = 0; i < points_pos.size(); i++)
	{
		int next = (i + 1) % points_pos.size();
		Visualizer::renderLine3D(points_pos[i], points_pos[next], vec4_white);
	}
}

void SimpleTrajectoryMovement::update_time()
{
	float len = (float)length(points_pos[points_index] - prev_point);

	// update time with stable velocity
	time += velocity / len * Game::getIFps();
	if (time >= 1.0f)
	{
		prev_point = points_pos[points_index];
		prev_rot = points_rot[points_index];

		// next point
		points_index = (points_index + ftoi(time)) % points_pos.size();
		time = Math::frac(time);
	}
}

void SplineTrajectoryMovement::init()
{
	UNIGINE_ASSERT(!pathNode.nullCheck());

	// save positions and rotations
	int num_childs = pathNode->getNumChildren();
	for (int i = 0; i < num_childs; i++)
	{
		NodePtr nc = pathNode->getChild(i);
		points_pos.emplace_back(nc->getWorldPosition());
		points_rot.emplace_back(nc->getWorldRotation());
	}

	// save the length of the spline segments to calculate the stable speed
	lengths.clear();
	int points_count = points_pos.size();
	for (int j = 0; j < points_count; j++)
	{
		int j_prev = (j - 1 < 0) ? (points_count - 1) : j - 1;
		int j_cur = j;
		int j_next = (j + 1) % points_count;
		int j_next_next = (j + 2) % points_count;

		const Vec3& p0 = points_pos[j_prev];
		const Vec3& p1 = points_pos[j_cur];
		const Vec3& p2 = points_pos[j_next];
		const Vec3& p3 = points_pos[j_next_next];

		lengths.append(Utils::getLengthCatmullRomCentripetal(p0, p1, p2, p3, quality));
	}
}

void SplineTrajectoryMovement::update()
{
	// update time
	float speed = velocity / (lengths[points_index][int(time * (quality - 1))] * quality);
	update_time(speed);

	// get current control points
	VectorStack<Vec3, 4> p = get_current_points();
	VectorStack<quat, 4> q = get_current_quats();

	// spline calculations
	Vec3 pos = Utils::catmullRomCentripetal(p[0], p[1], p[2], p[3], time);
	quat rot = Utils::squad(q[0], q[1], q[2], q[3], time);

	// change node pos
	node->setWorldPosition(pos);
	node->setWorldRotation(rot, true);

	if (debug.get())
		visualize_path();
}

void SplineTrajectoryMovement::visualize_path()
{
	int points_count = points_pos.size();
	for (int j = 0; j < points_count; j++)
	{
		int j_prev = (j - 1 < 0) ? (points_count - 1) : j - 1;
		int j_cur = j;
		int j_next = (j + 1) % points_count;
		int j_next_next = (j + 2) % points_count;

		const Vec3& p0 = points_pos[j_prev];
		const Vec3& p1 = points_pos[j_cur];
		const Vec3& p2 = points_pos[j_next];
		const Vec3& p3 = points_pos[j_next_next];

		// draw curve
		Vec3 start = Utils::catmullRomCentripetal(p0, p1, p2, p3, 0);
		int quality = 10;
		for (int i = 1; i < quality; i++)
		{
			Vec3 end = Utils::catmullRomCentripetal(p0, p1, p2, p3, float(i) / (quality - 1));
			Visualizer::renderLine3D(start, end, vec4_white);
			start = end;
		}
	}
}

void SplineTrajectoryMovement::update_time(float speed)
{
	time += speed * Game::getIFps();
	if (time >= 1.0f)
	{
		points_index = (points_index + ftoi(time)) % points_pos.size(); // loop
		time = Math::frac(time);
	}
}

VectorStack<Vec3, 4> SplineTrajectoryMovement::get_current_points() const
{
	int points_count = points_pos.size();
	int i_prev = (points_index - 1 < 0) ? (points_count - 1) : points_index - 1;
	int i_cur = points_index;
	int i_next = (points_index + 1) % points_count;
	int i_next_next = (points_index + 2) % points_count;

	VectorStack<Vec3, 4> result;
	result.append(points_pos[i_prev]);
	result.append(points_pos[i_cur]);
	result.append(points_pos[i_next]);
	result.append(points_pos[i_next_next]);
	return result;
}

VectorStack<quat, 4> SplineTrajectoryMovement::get_current_quats() const
{
	int points_count = points_pos.size();
	int i_prev = (points_index - 1 < 0) ? (points_count - 1) : points_index - 1;
	int i_cur = points_index;
	int i_next = (points_index + 1) % points_count;
	int i_next_next = (points_index + 2) % points_count;

	VectorStack<quat, 4> result;
	result.append(points_rot[i_prev]);
	result.append(points_rot[i_cur]);
	result.append(points_rot[i_next]);
	result.append(points_rot[i_next_next]);
	return result;
}


void PathTrajectorySaver::init()
{
	UNIGINE_ASSERT(!path_file.nullCheck());
	UNIGINE_ASSERT(!path_node.nullCheck());

	// calculate and save spline in .path file
	if (autosave.get())
		save();
}

void PathTrajectorySaver::save()
{
	PathPtr path = Path::create();
	path->clear();

	int points_count = path_node->getNumChildren();
	double frame_time = 0;
	for (int j = 0; j < points_count; j++)
	{
		int j_prev = (j - 1 < 0) ? (points_count - 1) : j - 1;
		int j_cur = j;
		int j_next = (j + 1) % points_count;
		int j_next_next = (j + 2) % points_count;

		// get current control position and rotation
		const Vec3 p0 = path_node->getChild(j_prev)->getWorldPosition();
		const Vec3 p1 = path_node->getChild(j_cur)->getWorldPosition();
		const Vec3 p2 = path_node->getChild(j_next)->getWorldPosition();
		const Vec3 p3 = path_node->getChild(j_next_next)->getWorldPosition();

		const quat q0 = path_node->getChild(j_prev)->getWorldRotation();
		const quat q1 = path_node->getChild(j_cur)->getWorldRotation();
		const quat q2 = path_node->getChild(j_next)->getWorldRotation();
		const quat q3 = path_node->getChild(j_next_next)->getWorldRotation();

		// calculate curve
		Vec3 start = Utils::catmullRomCentripetal(p0, p1, p2, p3, 0);
		for (int i = 1; i < quality; i++)
		{
			path->addFrame();

			float time = float(i) / (quality - 1);

			// calculate segment position and rotation
			Vec3 end = Utils::catmullRomCentripetal(p0, p1, p2, p3, time);
			quat rot = Utils::squad(q0, q1, q2, q3, time);

			// calculate current frame time
			double len = length(start - end);
			frame_time += len;

			path->setFramePosition(path->getNumFrames() - 1, end);
			path->setFrameRotation(path->getNumFrames() - 1, rot);
			path->setFrameTime(path->getNumFrames() - 1, static_cast<float>(frame_time));

			start = end;
		}
	}

	// save to file
	path->save(path_file.get());
}

void SavedPathTrajectory::init()
{
	node->setWorldPosition(Vec3(0, 0, 0));

	transform_path = WorldTransformPath::create(trajectory_file_path);
	transform_path->setLoop(1);
	transform_path->setTime(0.0f);
	transform_path->setSpeed(velocity);
	transform_path->play();
	transform_path->addChild(node);
}

void SavedPathTrajectory::update()
{
	transform_path->setSpeed(velocity);

	if (debug.get())
		visualize_path();
}

void SavedPathTrajectory::visualize_path()
{
	PathPtr path = transform_path->getPath();
	int num_frames = path->getNumFrames();
	for (int i = 0; i < num_frames; i++)
	{
		Vec3 curr_point = path->getFramePosition(i);
		Vec3 next_point = path->getFramePosition((i + 1) % num_frames);

		Visualizer::renderLine3D(curr_point, next_point, vec4_white);
	}
}
