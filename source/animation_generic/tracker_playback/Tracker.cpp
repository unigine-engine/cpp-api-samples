// Provides a C++ interface to the Tracker system via UnigineScript wrapper.
// Tracks are animation files (.track) that can be loaded, queried for timing info,
// and evaluated at specific times. The wrapper script handles actual track playback.

#include "Tracker.h"
#include <UnigineFileSystem.h>
#include <UnigineEngine.h>

REGISTER_COMPONENT(Tracker);

using namespace Unigine;
using namespace Math;

bool Tracker::is_wrapper_loaded = false;

// Function names for calling into the UnigineScript TrackerWrapper
const Variable Tracker::init_func = Variable("TrackerWrapper::init");
const Variable Tracker::shutdown_func = Variable("TrackerWrapper::shutdown");
const Variable Tracker::add_track_func = Variable("TrackerWrapper::addTrack");
const Variable Tracker::remove_track_func = Variable("TrackerWrapper::removeTrack");
const Variable Tracker::get_min_time_func = Variable("TrackerWrapper::getMinTime");
const Variable Tracker::get_max_time_func = Variable("TrackerWrapper::getMaxTime");
const Variable Tracker::get_unit_time_func = Variable("TrackerWrapper::getUnitTime");
const Variable Tracker::set_func = Variable("TrackerWrapper::set");

Map<String, int> Tracker::track_ids;

Variable Tracker::track_file_var = Variable("");
Variable Tracker::track_id_var = Variable(0);
Variable Tracker::time_var = Variable(0.0f);

bool Tracker::is_initialized = false;

// Loads a .track file and returns its ID for subsequent operations.
// Track name is extracted from filename for lookup in the track_ids map.
int Tracker::addTrack(const char *track_file)
{
	if (isTrackerInitialized() && FileSystem::isFileExist(track_file) &&
		!String::compare(FileSystem::getExtension(track_file), "track"))
	{
		// Extract track name from path: "folder/trackname.track" -> "trackname"
		auto track_name = String::split(track_file, ".")[0];
		auto parts = String::split(track_name, "/");
		track_name = parts[parts.size() - 1];

		if (!track_ids.contains(track_name))
		{
			// Call script wrapper to load the track and get its ID
			track_file_var.setString(track_file);
			int track_id = Engine::get()->runWorldFunction(add_track_func, track_file_var).getInt();
			track_ids.insert(track_name, track_id);

			return track_id;
		}
		else
		{
			Log::warning("Tracker::addTrack: %s already added\n", track_file);
			return track_ids[track_name];
		}
	}

	return -1;
}

// Track is unloaded and removed from the track_ids map by ID.
void Tracker::removeTrack(int track_id)
{
	if (isTrackerInitialized())
	{
		auto it = track_ids.findData(track_id);
		if (it != track_ids.end())
		{
			track_id_var.setInt(track_id);
			Engine::get()->runWorldFunction(remove_track_func, track_id_var);

			track_ids.remove(it);
		}
	}
}

// Track is unloaded and removed from the track_ids map by name.
void Tracker::removeTrack(const char* track_name)
{
	if (isTrackerInitialized() && track_ids.contains(track_name))
		removeTrack(track_ids[track_name]);
}

// Returns true if a track with the given name is loaded.
bool Tracker::containsTrack(const char* track_name)
{
	if (isTrackerInitialized())
		return track_ids.contains(track_name);

	return false;
}

// Track ID is returned for the given track name, or -1 if not found.
int Tracker::getTrackID(const char* track_name)
{
	if (isTrackerInitialized() && track_ids.contains(track_name))
		return track_ids[track_name];

	return -1;
}

// Minimum time (first keyframe) of the track is returned.
float Tracker::getMinTime(int track_id)
{
	if (isTrackerInitialized())
	{
		track_id_var.setInt(track_id);
		return Engine::get()->runWorldFunction(get_min_time_func, track_id_var).getFloat();
	}

	return 0.0f;
}

// Minimum time is returned by track name lookup.
float Tracker::getMinTime(const char* track_name)
{
	if (isTrackerInitialized() && track_ids.contains(track_name))
		return getMinTime(track_ids[track_name]);

	return 0.0f;
}

// Maximum time (last keyframe) of the track is returned.
float Tracker::getMaxTime(int track_id)
{
	if (isTrackerInitialized())
	{
		track_id_var.setInt(track_id);
		return Engine::get()->runWorldFunction(get_max_time_func, track_id_var).getFloat();
	}

	return 0.0f;
}

// Maximum time is returned by track name lookup.
float Tracker::getMaxTime(const char* track_name)
{
	if (isTrackerInitialized() && track_ids.contains(track_name))
		return getMaxTime(track_ids[track_name]);

	return 0.0f;
}

// Unit time (time scale, frames per second) of the track is returned.
float Tracker::getUnitTime(int track_id)
{
	if (isTrackerInitialized())
	{
		track_id_var.setInt(track_id);
		return Engine::get()->runWorldFunction(get_unit_time_func, track_id_var).getFloat();
	}

	return 0.0f;
}

// Unit time is returned by track name lookup.
float Tracker::getUnitTime(const char* track_name)
{
	if (isTrackerInitialized() && track_ids.contains(track_name))
		return getUnitTime(track_ids[track_name]);

	return 0.0f;
}

// Track is evaluated at the given time and animation is applied.
void Tracker::setTime(int track_id, float time)
{
	if (isTrackerInitialized())
	{
		time_var.setFloat(time);
		track_id_var.setInt(track_id);
		Engine::get()->runWorldFunction(set_func, track_id_var, time_var);
	}
}

// Track time is set by track name lookup.
void Tracker::setTime(const char* track_name, float time)
{
	if (isTrackerInitialized() && track_ids.contains(track_name))
		setTime(track_ids[track_name], time);
}

// TrackerWrapper script is verified and initialized, then tracks are loaded.
void Tracker::init()
{
	// Verify the TrackerWrapper script is assigned as world logic
	String tracker_wrapper_guid = FileSystem::guidToPath(FileSystem::getGUID(tracker_wrapper_usc));
	is_wrapper_loaded = (String::compare(World::getScriptName(), tracker_wrapper_guid) == 0);

	// If wrapper not loaded, assign it and reload the world to apply changes
	if (!is_wrapper_loaded)
	{
		World::setScriptName(tracker_wrapper_guid);
		World::setScriptExecute(true);
		World::saveWorld();
		World::reloadWorld();
		return;
	}

	// Initialize the tracker system in the script wrapper
	Engine::get()->runWorldFunction(init_func);
	is_initialized = true;

	// Load all tracks specified in the component property array
	if (!track_files.nullCheck())
	{
		for (int i = 0; i < track_files.size(); i++)
			addTrack(track_files[i].get());
	}
}

// Tracker system is shut down and all tracks are unloaded.
void Tracker::shutdown()
{
	if (isTrackerInitialized())
	{
		Engine::get()->runWorldFunction(shutdown_func);

		track_ids.clear();
		is_wrapper_loaded = false;
		is_initialized = false;
	}
}
