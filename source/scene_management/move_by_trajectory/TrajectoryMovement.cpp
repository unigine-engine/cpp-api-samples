// Trajectory-based movement implementations. Four variants are provided:
// - SimpleTrajectoryMovement: Linear interpolation between waypoints
// - SplineTrajectoryMovement: Smooth Catmull-Rom spline interpolation with SQUAD rotation
// - PathTrajectorySaver: Converts waypoints to .path file format
// - SavedPathTrajectory: Plays back .path files using WorldTransformPath

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

// Waypoint positions and rotations are cached from child nodes.
void SimpleTrajectoryMovement::init()
{
	UNIGINE_ASSERT(!path_node.nullCheck());

	// Cache waypoint transforms from path node children
	int num_childs = path_node->getNumChildren();
	for (int i = 0; i < num_childs; i++)
	{
		NodePtr nc = path_node->getChild(i);
		points_pos.emplace_back(nc->getWorldPosition());
		points_rot.emplace_back(nc->getWorldRotation());
	}
	prev_point = node->getWorldPosition();
}

// Position and rotation are linearly interpolated each frame.
void SimpleTrajectoryMovement::update()
{
	update_time();

	// Lerp position, slerp rotation between current segment endpoints
	node->setWorldPosition(lerp(prev_point, points_pos[points_index], time));
	node->setWorldRotation(slerp(prev_rot, points_rot[points_index], time));

	if (debug.get())
		visualize_path();
}

// Draws lines connecting waypoints in a closed loop.
void SimpleTrajectoryMovement::visualize_path()
{
	for (int i = 0; i < points_pos.size(); i++)
	{
		int next = (i + 1) % points_pos.size();
		Visualizer::renderLine3D(points_pos[i], points_pos[next], vec4_white);
	}
}

// Time is advanced based on velocity and segment length for constant speed.
void SimpleTrajectoryMovement::update_time()
{
	float len = (float)length(points_pos[points_index] - prev_point);

	// Update interpolation time normalized by segment length
	time += velocity / len * Game::getIFps();
	if (time >= 1.0f)
	{
		// Cache current point as segment start for next interpolation
		prev_point = points_pos[points_index];
		prev_rot = points_rot[points_index];

		// Advance to next waypoint with loop wrapping
		points_index = (points_index + ftoi(time)) % points_pos.size();
		time = Math::frac(time);
	}
}

// Waypoints are cached and segment lengths are precomputed for constant velocity.
void SplineTrajectoryMovement::init()
{
	UNIGINE_ASSERT(!pathNode.nullCheck());

	// Cache waypoint transforms from path node children
	int num_childs = pathNode->getNumChildren();
	for (int i = 0; i < num_childs; i++)
	{
		NodePtr nc = pathNode->getChild(i);
		points_pos.emplace_back(nc->getWorldPosition());
		points_rot.emplace_back(nc->getWorldRotation());
	}

	// Precompute segment arc lengths for constant-velocity movement
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

// Spline position and SQUAD rotation are evaluated each frame.
void SplineTrajectoryMovement::update()
{
	// Calculate speed using precomputed segment length for constant velocity
	float speed = velocity / (lengths[points_index][int(time * (quality - 1))] * quality);
	update_time(speed);

	// Gather four control points for Catmull-Rom spline evaluation
	VectorStack<Vec3, 4> p = get_current_points();
	VectorStack<quat, 4> q = get_current_quats();

	// Evaluate position on Catmull-Rom spline and rotation with SQUAD
	Vec3 pos = Utils::catmullRomCentripetal(p[0], p[1], p[2], p[3], time);
	quat rot = Utils::squad(q[0], q[1], q[2], q[3], time);

	// Apply interpolated transform to node
	node->setWorldPosition(pos);
	node->setWorldRotation(rot, true);

	if (debug.get())
		visualize_path();
}

// Draws smooth spline curves between all waypoints.
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

		// Draw spline segment as connected line strips
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

// Interpolation time is advanced; wraps to next segment when complete.
void SplineTrajectoryMovement::update_time(float speed)
{
	time += speed * Game::getIFps();
	if (time >= 1.0f)
	{
		// Advance to next segment with loop wrapping
		points_index = (points_index + ftoi(time)) % points_pos.size();
		time = Math::frac(time);
	}
}

// Gathers four surrounding position control points for spline evaluation.
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

// Gathers four surrounding rotation control points for SQUAD evaluation.
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


// Path file is generated automatically if autosave is enabled.
void PathTrajectorySaver::init()
{
	UNIGINE_ASSERT(!path_file.nullCheck());
	UNIGINE_ASSERT(!path_node.nullCheck());

	// Generate .path file on component initialization if enabled
	if (autosave.get())
		save();
}

// Spline is evaluated and frame data is written to .path file.
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

		// Gather four control points for Catmull-Rom evaluation
		const Vec3 p0 = path_node->getChild(j_prev)->getWorldPosition();
		const Vec3 p1 = path_node->getChild(j_cur)->getWorldPosition();
		const Vec3 p2 = path_node->getChild(j_next)->getWorldPosition();
		const Vec3 p3 = path_node->getChild(j_next_next)->getWorldPosition();

		const quat q0 = path_node->getChild(j_prev)->getWorldRotation();
		const quat q1 = path_node->getChild(j_cur)->getWorldRotation();
		const quat q2 = path_node->getChild(j_next)->getWorldRotation();
		const quat q3 = path_node->getChild(j_next_next)->getWorldRotation();

		// Sample spline at quality intervals to generate path frames
		Vec3 start = Utils::catmullRomCentripetal(p0, p1, p2, p3, 0);
		for (int i = 1; i < quality; i++)
		{
			path->addFrame();

			float time = float(i) / (quality - 1);

			// Evaluate spline position and SQUAD rotation at current t
			Vec3 end = Utils::catmullRomCentripetal(p0, p1, p2, p3, time);
			quat rot = Utils::squad(q0, q1, q2, q3, time);

			// Accumulate arc length for frame timing (enables constant velocity playback)
			double len = length(start - end);
			frame_time += len;

			path->setFramePosition(path->getNumFrames() - 1, end);
			path->setFrameRotation(path->getNumFrames() - 1, rot);
			path->setFrameTime(path->getNumFrames() - 1, static_cast<float>(frame_time));

			start = end;
		}
	}

	// Save to file
	path->save(path_file.get());
}

// WorldTransformPath is created and node is attached for automatic animation.
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

// Playback speed is updated and path is visualized if debug is enabled.
void SavedPathTrajectory::update()
{
	transform_path->setSpeed(velocity);

	if (debug.get())
		visualize_path();
}

// Draws lines connecting all frames in the loaded path.
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
