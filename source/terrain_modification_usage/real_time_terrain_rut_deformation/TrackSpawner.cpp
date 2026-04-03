// Spawns track decal nodes at regular intervals as parent node moves.
// Manages a pool of track nodes that are reused in circular fashion.
// Track spacing and maximum count are configurable at runtime.

#include "TrackSpawner.h"

#include <UnigineWorld.h>

REGISTER_COMPONENT(TrackSpawner);

using namespace Unigine;
using namespace Math;

// Track pool is cleared and new maximum is set.
void TrackSpawner::setMaxNumberOfTracks(int max_number)
{
	// Existing tracks are scheduled for deletion
	for (const auto &track : active_tracks)
		track.deleteLater();
	active_tracks.clear();

	// New limit is applied and reuse index is reset
	max_number_of_tracks = max_number;
	next_active_track_to_reuse = 0;
}

// Parameters are initialized from property values.
void TrackSpawner::init()
{
	// Runtime parameters are set from editor properties
	setMinDistanceBetweenTracks(min_distance_between_tracks_param.get());
	setMaxNumberOfTracks(max_number_of_tracks_param.get());
}

// Track is spawned or reused when distance threshold is exceeded.
void TrackSpawner::update()
{
	// No tracks are spawned if limit is zero
	if (max_number_of_tracks == 0)
		return;

	// Squared distance is computed for efficiency
	auto world_position = node->getWorldPosition();
	auto distance2 = (last_track_position - world_position).length2();

	// Track is skipped if too close to previous position
	if (distance2 < min_distance_between_tracks * min_distance_between_tracks)
		return;

	// Current position is stored for next distance check
	last_track_position = world_position;

	NodePtr current_track;
	if (active_tracks.size() < max_number_of_tracks)
	{
		// New track is loaded when pool has capacity
		current_track = World::loadNode(track_node.get());
		active_tracks.push_back(current_track);
	}
	else
	{
		// Existing track is reused in circular order
		current_track = active_tracks[next_active_track_to_reuse];
		next_active_track_to_reuse = (next_active_track_to_reuse + 1) % active_tracks.size();
	}

	// Track is positioned at current spawner location
	current_track->setTransform(node->getWorldTransform());
}
