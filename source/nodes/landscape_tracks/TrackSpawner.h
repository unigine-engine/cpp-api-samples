#pragma once

#include <UnigineComponentSystem.h>

class TrackSpawner : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrackSpawner, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);


	PROP_PARAM(File, track_node, "Track node")
	PROP_PARAM(Float, min_distance_between_tracks_param, 1.0f, "Min distance between tracks");
	PROP_PARAM(Int, max_number_of_tracks_param, 100, "Max number of tracks");


	void setMinDistanceBetweenTracks(float min_distance) { min_distance_between_tracks = min_distance; }
	void setMaxNumberOfTracks(int max_number);


private:
	void init();
	void update();

	float min_distance_between_tracks{0.1f};
	int max_number_of_tracks{100};

	Unigine::Math::Vec3 last_track_position = Unigine::Math::Vec3_inf;

	Unigine::Vector<Unigine::NodePtr> active_tracks;
	int next_active_track_to_reuse = 0;
};