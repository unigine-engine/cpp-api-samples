#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineInterpreter.h>
#include <UnigineMap.h>

class TrackPlayback : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TrackPlayback, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

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