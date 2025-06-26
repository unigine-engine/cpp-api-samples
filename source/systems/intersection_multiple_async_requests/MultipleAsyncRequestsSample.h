#pragma once

#include "../../menu_ui/SampleDescriptionWindow.h"
#include "../../utils/intersection/Intersections.h"

#include <UnigineComponentSystem.h>
#include <UnigineWidgets.h>

class MultipleAsyncRequestsSample : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(MultipleAsyncRequestsSample, Unigine::ComponentBase);
	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_PARAM(Float, intersection_distance, 10000.0f);

private:
	void init();
	void update();
	void shutdown();

	void on_intersection_end(IntersectionRequest *r);

private:
	const int num_slices{30};
	const int num_stacks{30};

	const float min_height{35.0f};
	const float max_height{200.0f};

	Unigine::Math::Vec3 emitter_pos{500.0f, 500.0f, 100.0f};
	float emitter_rotation{0.0f};

	Unigine::Vector<IntersectionRequest *> requests;

	// buffers for completed fetches
	Unigine::Vector<IntersectionRequest *> *buffer{nullptr};
	Unigine::Vector<IntersectionRequest *> first_buffer;
	Unigine::Vector<IntersectionRequest *> second_buffer;
	Unigine::Mutex mutex;

	struct Result
	{
		bool is_intersected{false};
		Unigine::Math::Vec3 point;
		Unigine::Math::vec3 normal;
	};

	Unigine::Vector<Result> results;

	// UI
	void init_gui();
	SampleDescriptionWindow sample_description_window;

	long long immediate_latency_count{0};
	long long low_latency_count{0};
	long long middle_latency_count{0};
	long long high_latency_count{0};
};
