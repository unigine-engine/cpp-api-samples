#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInterpreter.h>
#include <UnigineMap.h>

// Demonstrates playback of multiple Tracker animation tracks.
// Shows two access patterns: by track ID (faster) and by track name (more readable).
// Tracks for position and rotation are loaded from property, scale track is loaded at runtime.
class TrackPlayback : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrackPlayback, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	// Additional track to load dynamically (position and rotation tracks are in Tracker component)
	PROP_PARAM(File, scale_track_path);

private:
	void init();
	void update();

	int position_track_id{ -1 };
	int scale_track_id{ -1 };

	float position_track_time{ 0.0f };
	float rotation_track_time{ 0.0f };
	float scale_track_time{ 0.0f };
};