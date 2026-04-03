// Casts a single async intersection ray from camera through mouse cursor each frame.
// Hit point and normal are visualized; latency statistics are computed and displayed.

#include "SimpleAsyncRequestSample.h"

#include <UniginePlayers.h>
#include <UnigineGame.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(SimpleAsyncRequestSample);

using namespace Unigine;
using namespace Math;

// UI is created and visualizer is enabled.
void SimpleAsyncRequestSample::init()
{
	init_gui();
	Visualizer::setEnabled(true);
}

// Ray is cast from player through mouse; result is visualized and latency is tracked.
void SimpleAsyncRequestSample::update()
{
	// Get the main player in the scene
	PlayerPtr main_player = Game::getPlayer();
	if (main_player)
	{
		// If the previous intersection request has completed, read result and start a new one
		if (is_player_completed)
		{
			// Record pipeline latency in milliseconds (measured in update where Game::getTime is current)
			if (warmup_frames > 0)
			{
				warmup_frames--;
			}
			else if (player_request_time > 0.0)
			{
				double latency_ms = (Game::getTime() - player_request_time) * 1000.0;

				player_latency[player_latency_index] = latency_ms;
				player_latency_index++;
				if (player_latency_index == PLAYER_LATENCY_COUNT)
					player_latency_index = 0;
				if (player_latency_filled < PLAYER_LATENCY_COUNT)
					player_latency_filled++;

				// Update all-time max and compute average latency from the buffer
				if (latency_ms > max_player_latency)
					max_player_latency = latency_ms;

				avg_player_latency = 0.0;
				for (int i = 0; i < player_latency_filled; i++)
					avg_player_latency += player_latency[i];
				avg_player_latency /= player_latency_filled;
			}

			// Mark request as in progress
			is_player_completed = false;

			Vec3 p0 = main_player->getWorldPosition();
			const auto mouse_coord = Input::getMousePosition();
			Vec3 p1 = p0 + Vec3(main_player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y)) * intersection_distance.get();

			// Capture a smart pointer to prevent the lambda from accessing a deleted node
			NodePtr check_deleted = node;

			// Record submission time and perform an asynchronous intersection request
			player_request_time = Game::getTime();
			Intersections::getAsync(p0, p1, 1, [this, check_deleted](IntersectionRequest *r)
			{
				if (!check_deleted)
					return;

				is_player_completed = true;

				// Store intersection result
				if (r->isIntersection())
				{
					player_result.point = r->getPoint();
					player_result.normal = r->getNormal();
				}
				player_result.is_intersected = r->isIntersection();
			});
		}

		// Render a vector at the intersection point to visualize the result
		if (player_result.is_intersected)
			Visualizer::renderVector(player_result.point, player_result.point + Vec3(player_result.normal) * 10.0f, vec4_green);
	}

	String text = String::format(
		"Latency (ms per result)\nPlayer: avg %.3f ms, max %.3f ms\n\n",
		avg_player_latency,
		max_player_latency
	);
	sample_description_window.setStatus(text.get());
}

// Visualizer is disabled and UI window is closed.
void SimpleAsyncRequestSample::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

// UI window is created.
void SimpleAsyncRequestSample::init_gui()
{
	sample_description_window.createWindow();
}
