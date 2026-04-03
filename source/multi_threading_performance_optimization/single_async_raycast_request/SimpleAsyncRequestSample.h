#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/intersection/Intersections.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// Demonstrates single async intersection query from camera through mouse cursor.
// Tracks time-based latency statistics (average and maximum) for performance analysis.
class SimpleAsyncRequestSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(SimpleAsyncRequestSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component performs an asynchronous intersection from the player toward the mouse cursor, "
				"visualizes the hit with a normal vector, and displays average and maximum time latency in the UI.")

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Maximum distance to check for intersections from the player (set in the Editor)
	PROP_PARAM(Float, intersection_distance, 10000.0f);

private:
	void init();     // Enables visualizer and creates UI
	void update();   // Casts ray and displays intersection result
	void shutdown(); // Disables visualizer and closes UI

private:
	// Structure to store the result of an intersection request
	struct Result
	{
		bool is_intersected{false};
		Unigine::Math::Vec3 point;
		Unigine::Math::vec3 normal;
	};

	// Player-related state
	bool is_player_completed{true};					// Previous request state
	Result player_result;							// Latest intersection result
	double player_request_time{0.0};				// Time when the last async request was submitted
	int warmup_frames{5};							// Skip initial frames to avoid load spikes

	// Sample UI with description and controls
	void init_gui();
	SampleDescriptionWindow sample_description_window;

	// Latency tracking for requests (in milliseconds)
	enum
	{
		PLAYER_LATENCY_COUNT = 600							// Number of samples to keep latency history
	};
	double player_latency[PLAYER_LATENCY_COUNT]{0.0};		// Circular buffer (ms)
	double avg_player_latency{0.0};							// Average latency over the buffer (ms)
	double max_player_latency{0.0};							// Maximum latency observed (ms)
	int player_latency_index{0};							// Current index in the buffer
	int player_latency_filled{0};							// Number of valid entries in the buffer
};
