#include "TrackSpawner.h"

#include <UnigineWorld.h>

REGISTER_COMPONENT(TrackSpawner);

using namespace Unigine;
using namespace Math;


void TrackSpawner::setMaxNumberOfTracks(int max_number)
{
	for (const auto &track : active_tracks)
		track.deleteLater();
	active_tracks.clear();
	max_number_of_tracks = max_number;
	next_active_track_to_reuse = 0;
}

void TrackSpawner::init()
{
	setMinDistanceBetweenTracks(min_distance_between_tracks_param.get());
	setMaxNumberOfTracks(max_number_of_tracks_param.get());
}

void TrackSpawner::update()
{
	if (max_number_of_tracks == 0)
		return;

	auto world_position = node->getWorldPosition();
	auto distance2 = (last_track_position - world_position).length2();

	if (distance2 < min_distance_between_tracks * min_distance_between_tracks)
		return;

	last_track_position = world_position;

	NodePtr current_track;
	if (active_tracks.size() < max_number_of_tracks)
	{
		current_track = World::loadNode(track_node.get());
		active_tracks.push_back(current_track);
	}
	else
	{
		current_track = active_tracks[next_active_track_to_reuse];
		next_active_track_to_reuse = (next_active_track_to_reuse + 1) % active_tracks.size();
	}

	current_track->setTransform(node->getWorldTransform());
}
