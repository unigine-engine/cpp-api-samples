#include "MultipleAsyncRequestsSample.h"

#include <UniginePlayers.h>
#include <UnigineGame.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(MultipleAsyncRequestsSample);

using namespace Unigine;
using namespace Math;

void MultipleAsyncRequestsSample::init()
{
	init_gui();

	requests.resize(num_slices * num_stacks);
	for (int i = 0; i < requests.size(); i++)
	{
		requests[i] = new IntersectionRequest();
		requests[i]->addAsyncEndCallback(MakeCallback(this, &MultipleAsyncRequestsSample::on_intersection_end));
	}

	results.resize(num_slices * num_stacks);

	second_buffer = requests;
	buffer = &second_buffer;

	Visualizer::setEnabled(true);
}

void MultipleAsyncRequestsSample::update()
{
	// update emitter transform
	float k = (Math::sin(0.3f * Game::getTime()) + 1.0f) / 2.0f;
	emitter_pos.z = min_height + k * (max_height - min_height);

	emitter_rotation += 10.0f * Game::getIFps();
	if (emitter_rotation > 360.0f)
		emitter_rotation -= 360.0f;

	Visualizer::renderPoint3D(emitter_pos, 2.0f, vec4_red);

	// update emitter intersection results and rays
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

				Vec3 dir = rotateZ(slice_step * i + emitter_rotation) * Vec3_forward;
				dir = quat(cross(vec3(dir), vec3_up), -stack_step * j) * dir;

				fetch->setPositionBegin(emitter_pos);
				fetch->setPositionEnd(emitter_pos + dir * intersection_distance);
			}

			if (res.is_intersected)
				Visualizer::renderVector(res.point, res.point + Vec3(res.normal) * 5.0f, vec4_red);
		}
	}

	// swap completed featches buffers
	{
		ScopedLock lock(mutex);

		if (buffer == &first_buffer)
			buffer = &second_buffer;
		else
			buffer = &first_buffer;

		buffer->clear();
	}

	// start intersections detection
	if (buffer == &first_buffer)
		Intersections::getAsync(second_buffer);
	else
		Intersections::getAsync(first_buffer);

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

void MultipleAsyncRequestsSample::shutdown()
{
	sample_description_window.shutdown();
	Visualizer::setEnabled(false);

	Intersections::wait(requests);

	for (int i = 0; i < requests.size(); i++)
		delete requests[i];
	requests.clear();

	buffer = nullptr;
	first_buffer.clear();
	second_buffer.clear();

	results.clear();
}

void MultipleAsyncRequestsSample::on_intersection_end(IntersectionRequest *r)
{
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

void MultipleAsyncRequestsSample::init_gui()
{
	sample_description_window.createWindow();
}
