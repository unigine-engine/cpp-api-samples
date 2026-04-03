#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/intersection/Intersections.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

// Demonstrates high-volume async intersection queries with double buffering.
// An emitter casts rays in a spherical pattern; each ray uses async intersection.
// Latency statistics track how many frames pass between request and completion.
class MultipleAsyncRequestsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MultipleAsyncRequestsSample, Unigine::ComponentBase);
	COMPONENT_DESCRIPTION("This component demonstrates running many asynchronous intersection ray casts, "
		"using double buffering to manage completed requests, visualizing hits, and reporting frame-latency statistics.")

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Ray length for intersection tests (set in the Editor)
	PROP_PARAM(Float, intersection_distance, 10000.0f);

private:
	void init();     // Creates request pool and initializes double buffer
	void update();   // Animates emitter and processes intersection results
	void shutdown(); // Waits for pending requests and cleans up

	void on_intersection_end(IntersectionRequest *r); // Callback for async completion

private:
	// Ray grid resolution (azimuth and elevation)
	const int num_slices{30};
	const int num_stacks{30};

	// Emitter height limits for simple up/down motion
	const float min_height{35.0f};
	const float max_height{200.0f};

	// Emitter origin and rotation
	Unigine::Math::Vec3 emitter_pos{500.0f, 500.0f, 100.0f};
	float emitter_rotation{0.0f};

	// One async request per ray in the grid
	Unigine::Vector<IntersectionRequest *> requests;

	// Double buffering to avoid contention with async work
	Unigine::Vector<IntersectionRequest *> *buffer{nullptr};
	Unigine::Vector<IntersectionRequest *> first_buffer;
	Unigine::Vector<IntersectionRequest *> second_buffer;
	Unigine::Mutex mutex;

	// Cached hit data per ray
	struct Result
	{
		bool is_intersected{false};
		Unigine::Math::Vec3 point;
		Unigine::Math::vec3 normal;
	};

	Unigine::Vector<Result> results;

	// Sample UI with description and controls
	void init_gui();
	SampleDescriptionWindow sample_description_window;

	// Frame-latency counters for async completion stats
	long long immediate_latency_count{0};
	long long low_latency_count{0};
	long long middle_latency_count{0};
	long long high_latency_count{0};
};
