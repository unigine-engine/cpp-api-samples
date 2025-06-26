#include "TrackPlayback.h"
#include <UnigineFileSystem.h>
#include <UnigineEngine.h>
#include <UnigineGame.h>
#include "Tracker.h"

REGISTER_COMPONENT(TrackPlayback);

using namespace Unigine;
using namespace Math;

void TrackPlayback::init()
{
	if (!Tracker::isTrackerInitialized())
		return;

	// get position track id that was added to tracker in editor
	if (Tracker::containsTrack("position_track"))
	{
		position_track_id = Tracker::getTrackID("position_track");
		position_track_time = Tracker::getMinTime(position_track_id);
	}

	// get rotation track time using track name
	if (Tracker::containsTrack("rotation_track"))
		rotation_track_time = Tracker::getMinTime("rotation_track");

	// add new track to tracker
	scale_track_id = Tracker::addTrack(scale_track_path);
	if (scale_track_id != -1)
		scale_track_time = Tracker::getMinTime(scale_track_id);
}

void TrackPlayback::update()
{
	if (!Tracker::isTrackerInitialized())
		return;

	// set position track time using id
	if (position_track_id != -1)
	{
		float min_time = Tracker::getMinTime(position_track_id);
		float max_time = Tracker::getMaxTime(position_track_id);
		float unit_time = Tracker::getUnitTime(position_track_id);

		position_track_time += Game::getIFps() / unit_time;
		if (position_track_time >= max_time)
			position_track_time = min_time;

		Tracker::setTime(position_track_id, position_track_time);
	}

	// set rotation track time using track name
	if (Tracker::containsTrack("rotation_track"))
	{
		float min_time = Tracker::getMinTime("rotation_track");
		float max_time = Tracker::getMaxTime("rotation_track");
		float unit_time = Tracker::getUnitTime("rotation_track");

		rotation_track_time += Game::getIFps() / unit_time;
		if (rotation_track_time >= max_time)
			rotation_track_time = min_time;

		Tracker::setTime("rotation_track", rotation_track_time);
	}

	// update scale track time
	if (scale_track_id != -1)
	{
		float min_time = Tracker::getMinTime(scale_track_id);
		float max_time = Tracker::getMaxTime(scale_track_id);
		float unit_time = Tracker::getUnitTime(scale_track_id);

		scale_track_time += Game::getIFps() / unit_time;
		if (scale_track_time >= max_time)
			scale_track_time = min_time;

		Tracker::setTime(scale_track_id, scale_track_time);
	}
}
