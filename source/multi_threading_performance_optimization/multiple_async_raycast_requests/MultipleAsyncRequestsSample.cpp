// Stress-tests async intersection system with many simultaneous ray queries.
// Uses double buffering for thread-safe request management and tracks latency
// histogram to show how many frames each request takes to complete.

#include "MultipleAsyncRequestsSample.h"

#include <UniginePlayers.h>
#include <UnigineGame.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(MultipleAsyncRequestsSample);

using namespace Unigine;
using namespace Math;

// Request pool is allocated and double buffer system is initialized.
void MultipleAsyncRequestsSample::init()
{
	init_gui();

	// Keep per-ray results in a dense array parallel to 'requests' for cache-friendly access during rendering
	requests.resize(num_slices * num_stacks);
	for (int i = 0; i < requests.size(); i++)
	{
		requests[i] = new IntersectionRequest();
		requests[i]->addAsyncEndCallback(MakeCallback(this, &MultipleAsyncRequestsSample::on_intersection_end));
	}
	results.resize(num_slices * num_stacks);

	// Two-buffer system: while one buffer is processed by the async system, we rebuild the other from finished callbacks
	second_buffer = requests;
	buffer = &second_buffer;

	Visualizer::setEnabled(true);
}

// Emitter is animated, completed rays are visualized, and new requests are dispatched.
void MultipleAsyncRequestsSample::update()
{
	// Animate emitter (moving up/down and rotating)
	// The emitter defines the origin and directions for all intersection rays this frame
	float k = (Math::sin(0.3f * Game::getTime()) + 1.0f) / 2.0f;
	emitter_pos.z = min_height + k * (max_height - min_height);

	emitter_rotation += 10.0f * Game::getIFps();
	if (emitter_rotation > 360.0f)
		emitter_rotation -= 360.0f;

	Visualizer::renderPoint3D(emitter_pos, 2.0f, vec4_red);

	// Update rays: check results and restart requests
	float slice_step = 360.0f / num_slices;
	float stack_step = 85.0f / num_stacks;

	for (int i = 0; i < num_slices; i++)
	{
		for (int j = 0; j < num_stacks; j++)
		{
			IntersectionRequest *fetch = requests[i * num_stacks + j];
			Result &res = results[i * num_stacks + j];

			if (fetch->isAsyncCompleted())
			{
				if (fetch->isIntersection())
				{
					res.point = fetch->getPoint();
					res.normal = fetch->getNormal();
				}
				res.is_intersected = fetch->isIntersection();

				// Set new direction for next cast
				Vec3 dir = rotateZ(slice_step * i + emitter_rotation) * Vec3_forward;
				dir = quat(cross(vec3(dir), vec3_up), -stack_step * j) * dir;

				fetch->setPositionBegin(emitter_pos);
				fetch->setPositionEnd(emitter_pos + dir * intersection_distance);
			}

			// Draw normals for hits to provide immediate visual feedback
			if (res.is_intersected)
				Visualizer::renderVector(res.point, res.point + Vec3(res.normal) * 5.0f, vec4_red);
		}
	}

	// Swap the buffer that will collect newly finished requests this frame
	// Double buffering makes sure we don't change the container while the async system is using it
	{
		ScopedLock lock(mutex);

		if (buffer == &first_buffer)
			buffer = &second_buffer;
		else
			buffer = &first_buffer;

		buffer->clear();
	}

	// Start async intersections on the inactive buffer
	if (buffer == &first_buffer)
		Intersections::getAsync(second_buffer);
	else
		Intersections::getAsync(first_buffer);

	// Show latency stats in UI
	long long total_count = immediate_latency_count + low_latency_count + middle_latency_count + high_latency_count;
	if (total_count != 0)
	{
		float immediate = static_cast<float>(immediate_latency_count) / total_count * 100.0f;
		float low = static_cast<float>(low_latency_count) / total_count * 100.0f;
		float middle = static_cast<float>(middle_latency_count) / total_count * 100.0f;
		float high = static_cast<float>(high_latency_count) / total_count * 100.0f;

		String text = "Latency (number of frames per result): \n";
		text += String::format("0-2 (Immediate): %.1f", immediate) + "%\n";
		text += String::format("3-5 (Low): %.1f", low) + "%\n";
		text += String::format("5-7 (Middle): %.1f", middle) + "%\n";
		text += String::format(">7 (High): %.1f", high) + "%\n";

		sample_description_window.setStatus(text.get());
	}
}

// All pending requests are waited on and resources are freed.
void MultipleAsyncRequestsSample::shutdown()
{
	// Close UI and stop drawing
	sample_description_window.shutdown();
	Visualizer::setEnabled(false);

	// Wait for all requests and clean up
	Intersections::wait(requests);

	for (int i = 0; i < requests.size(); i++)
		delete requests[i];
	requests.clear();

	buffer = nullptr;
	first_buffer.clear();
	second_buffer.clear();

	results.clear();
}

// Latency is recorded and request is added to completion buffer.
void MultipleAsyncRequestsSample::on_intersection_end(IntersectionRequest *r)
{
	// Count how many frames it took to complete request
	long long d = r->getAsyncFrameLatency();
	if (d <= 2)
		immediate_latency_count++;
	else if (d <= 5)
		low_latency_count++;
	else if (d <= 7)
		middle_latency_count++;
	else
		high_latency_count++;

	ScopedLock lock(mutex);
	buffer->append(r);
}

// UI window is created for displaying latency statistics.
void MultipleAsyncRequestsSample::init_gui()
{
	sample_description_window.createWindow();
}
