#include "SimpleAsyncRequestSample.h"

#include <UniginePlayers.h>
#include <UnigineGame.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>

REGISTER_COMPONENT(SimpleAsyncRequestSample);

using namespace Unigine;
using namespace Math;

void SimpleAsyncRequestSample::init()
{
	init_gui();

	Visualizer::setEnabled(true);
}

void SimpleAsyncRequestSample::update()
{
	// update player intersection
	PlayerPtr main_player = Game::getPlayer();
	if (main_player)
	{
		if (is_player_completed)
		{
			is_player_completed = false;

			Vec3 p0 = main_player->getWorldPosition();
			const auto mouse_coord = Input::getMousePosition();
			Vec3 p1 = p0 + Vec3(main_player->getDirectionFromMainWindow(mouse_coord.x, mouse_coord.y)) * intersection_distance.get();

			NodePtr check_deleted = node; // capture the smart pointer to prevent the lambda from being called on a destroyed object
			Intersections::getAsync(p0, p1, 1, [this, check_deleted](IntersectionRequest *r)
			{
				if (!check_deleted)
					return;

				is_player_completed = true;

				if (r->isIntersection())
				{
					player_result.point = r->getPoint();
					player_result.normal = r->getNormal();
				}
				player_result.is_intersected = r->isIntersection();

				player_latency[player_latency_index] = r->getAsyncFrameLatency();
				player_latency_index++;
				if (player_latency_index == PLAYER_LATENCY_COUNT)
					player_latency_index = 0;

				avg_player_latency = 0.0f;
				max_player_latency = 0;
				for (int i = 0; i < PLAYER_LATENCY_COUNT; i++)
				{
					max_player_latency = max(max_player_latency, player_latency[i]);
					avg_player_latency += player_latency[i];
				}
				avg_player_latency /= PLAYER_LATENCY_COUNT;

			});
		}

		if (player_result.is_intersected)
			Visualizer::renderVector(player_result.point, player_result.point + Vec3(player_result.normal) * 10.0f, vec4_green);
	}

	String text = String::format("Latency (number of frames per result)\nPlayer: avg %f, max %lld\n\n", avg_player_latency, max_player_latency);
	sample_description_window.setStatus(text.get());
}

void SimpleAsyncRequestSample::shutdown()
{
	Visualizer::setEnabled(false);
	sample_description_window.shutdown();
}

void SimpleAsyncRequestSample::init_gui()
{
	sample_description_window.createWindow();
}
