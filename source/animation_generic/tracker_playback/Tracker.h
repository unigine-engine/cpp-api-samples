#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInterpreter.h>
#include <UnigineMap.h>

// C++ wrapper for the Tracker animation system (which is implemented in UnigineScript).
// Provides a static API to load, manage, and playback animation tracks created in the
// Tracker editor tool. Tracks are .track files that contain keyframed animations.
// This component initializes the TrackerWrapper script and manages track registration.
class Tracker : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(Tracker, Unigine::ComponentBase);
	COMPONENT_INIT(init, INT_MIN);		// Early init to be ready for other components
	COMPONENT_SHUTDOWN(shutdown, INT_MAX);	// Late shutdown after other components

	// Path to TrackerWrapper.usc script that implements the actual Tracker logic
	PROP_PARAM(File, tracker_wrapper_usc);
	// Track files to load on initialization
	PROP_ARRAY(File, track_files);

	static bool isTrackerInitialized() { return is_initialized; }

	// Track management: load/unload .track files
	static int addTrack(const char *track_file);
	static void removeTrack(int track_id);
	static void removeTrack(const char *track_name);
	static bool containsTrack(const char *track_name);
	static int getTrackID(const char *track_name);

	// Track timing: get min/max frame range and time scale
	static float getMinTime(int track_id);
	static float getMinTime(const char *track_name);
	static float getMaxTime(int track_id);
	static float getMaxTime(const char* track_name);
	static float getUnitTime(int track_id);	// Time scale (frames per second)
	static float getUnitTime(const char* track_name);
	// Set current playback time - this evaluates the track and applies animation
	static void setTime(int track_id, float time);
	static void setTime(const char* track_name, float time);

private:
	void init();
	void shutdown();

	static bool is_wrapper_loaded;

	static const Unigine::Variable init_func;
	static const Unigine::Variable shutdown_func;
	static const Unigine::Variable add_track_func;
	static const Unigine::Variable remove_track_func;
	static const Unigine::Variable get_min_time_func;
	static const Unigine::Variable get_max_time_func;
	static const Unigine::Variable get_unit_time_func;
	static const Unigine::Variable set_func;

	static Unigine::Map<Unigine::String, int> track_ids;

	static Unigine::Variable track_file_var;
	static Unigine::Variable track_id_var;
	static Unigine::Variable time_var;

	static bool is_initialized;
};