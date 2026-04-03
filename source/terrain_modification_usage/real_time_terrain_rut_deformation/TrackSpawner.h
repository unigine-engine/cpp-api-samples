// Spawns track decal nodes at regular intervals as parent node moves.
// Manages a pool of track nodes that are reused in circular fashion.
// Track spacing and maximum count are configurable at runtime.

#pragma once

#include <UnigineComponentSystem.h>

// Places track marks along the path of a moving node.
class TrackSpawner : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrackSpawner, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_PARAM(File, track_node, "Track node")									// Node file to spawn as track mark
	PROP_PARAM(Float, min_distance_between_tracks_param, 1.0f, "Min distance between tracks");	// Minimum spacing between tracks
	PROP_PARAM(Int, max_number_of_tracks_param, 100, "Max number of tracks");	// Maximum tracks in pool

	void setMinDistanceBetweenTracks(float min_distance) { min_distance_between_tracks = min_distance; }	// Sets track spacing
	void setMaxNumberOfTracks(int max_number);		// Clears pool and sets new maximum

private:
	void init();
	void update();

	float min_distance_between_tracks{0.1f};		// Current spacing threshold
	int max_number_of_tracks{100};					// Current pool size limit

	Unigine::Math::Vec3 last_track_position = Unigine::Math::Vec3_inf;	// Last track spawn position

	Unigine::Vector<Unigine::NodePtr> active_tracks;	// Pool of spawned track nodes
	int next_active_track_to_reuse = 0;					// Index for circular reuse
};