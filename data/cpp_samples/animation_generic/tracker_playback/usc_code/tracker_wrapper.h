#ifndef __TRACKER_WRAPPER_H__
#define __TRACKER_WRAPPER_H__

#include <core/unigine.h>
#include <core/systems/tracker/tracker.h>

namespace TrackerWrapper
{
	using Unigine::Tracker;
	
	Tracker tracker;
	int tracks_map[]; // key - id, value - TrackerTrack
	int track_id_counter = 0;

	void init()
	{
		tracker = new Tracker();
	}
	
	void shutdown() 
	{
		for (int i = 0; i < tracks_map.size(); i++)
			delete tracks_map[i];
		tracks_map.clear();

		delete tracker;
	}
	
	int addTrack(string file_name)
	{
		TrackerTrack track = tracker.loadTrack(file_name);

		int id = track_id_counter;
		tracks_map.append(id, track);
		track_id_counter++;

		return id;
	}

	void removeTrack(int trackID)
	{
		TrackerTrack t = tracks_map.get(trackID);
		tracks_map.remove(trackID);
		delete t;
	}

	float getMinTime(int trackID)
	{
		TrackerTrack t = tracks_map.get(trackID);
		return t.getMinTime();
	}
	
	float getMaxTime(int trackID)
	{
		TrackerTrack t = tracks_map.get(trackID);
		return t.getMaxTime();
	}
	
	float getUnitTime(int trackID)
	{
		TrackerTrack t = tracks_map.get(trackID);
		return t.getUnitTime();
	}
	
	void set(int trackID, float time)
	{
		TrackerTrack t = tracks_map.get(trackID);
		t.set(time);
	}

}

#endif