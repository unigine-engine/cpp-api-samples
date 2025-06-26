#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/intersection/Intersections.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class SimpleAsyncRequestSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleAsyncRequestSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, intersection_distance, 10000.0f);

private:
	void init();
	void update();
	void shutdown();

private:
	struct Result
	{
		bool is_intersected{false};
		Unigine::Math::Vec3 point;
		Unigine::Math::vec3 normal;
	};

	bool is_player_completed{true};
	Result player_result;

	// UI
	void init_gui();
	SampleDescriptionWindow sample_description_window;

	enum
	{
		PLAYER_LATENCY_COUNT = 600
	};
	long long player_latency[PLAYER_LATENCY_COUNT]{0};
	float avg_player_latency{0.0f};
	long long max_player_latency{0};
	int player_latency_index{0};
};
