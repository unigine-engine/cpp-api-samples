#include "TrackPlayback.h"
#include <UnigineFileSystem.h>
#include <UnigineEngine.h>
#include <UnigineGame.h>
#include "Tracker.h"

REGISTER_COMPONENT(TrackPlayback);

using namespace Unigine;
using namespace Math;

// Track IDs are obtained and initial playback times are set.
void TrackPlayback::init()
{
	if (!Tracker::isTrackerInitialized())
		return;

	// Access track by name, then cache ID for faster update() access
	if (Tracker::containsTrack("position_track"))
	{
		position_track_id = Tracker::getTrackID("position_track");
		position_track_time = Tracker::getMinTime(position_track_id);
	}

	// Alternative: access track by name only (simpler but slightly slower)
	if (Tracker::containsTrack("rotation_track"))
		rotation_track_time = Tracker::getMinTime("rotation_track");

	// Dynamically add a track at runtime (not pre-loaded in Tracker component)
	scale_track_id = Tracker::addTrack(scale_track_path);
	if (scale_track_id != -1)
		scale_track_time = Tracker::getMinTime(scale_track_id);
}

// All tracks are advanced and evaluated each frame in a manual playback loop.
void TrackPlayback::update()
{
	if (!Tracker::isTrackerInitialized())
		return;

	// Manual playback loop: advance time, wrap at end, apply to track
	// Using track ID (cached in init) for faster access
	if (position_track_id != -1)
	{
		float min_time = Tracker::getMinTime(position_track_id);
		float max_time = Tracker::getMaxTime(position_track_id);
		float unit_time = Tracker::getUnitTime(position_track_id);

		// Advance time based on frame time, scaled by track's unit time
		position_track_time += Game::getIFps() / unit_time;
		if (position_track_time >= max_time)
			position_track_time = min_time;

		// setTime() evaluates the track at given time and applies animation
		Tracker::setTime(position_track_id, position_track_time);
	}

	// Using track name (looked up each frame - simpler but slower)
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

	// Scale track (dynamically loaded)
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
