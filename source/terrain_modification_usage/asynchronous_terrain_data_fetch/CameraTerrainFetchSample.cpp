// Demonstrates asynchronous terrain data fetching using LandscapeFetch.
// Raycasts from camera through mouse cursor to terrain surface.
// Displays terrain height, normal, and mask values at intersection point.

#include "CameraTerrainFetchSample.h"

#include <UnigineVisualizer.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(CameraTerrainFetchSample)

// Visualizer is enabled; player reference is obtained from attached node.
void CameraTerrainFetchSample::init()
{
	// Visualizer state is saved for restoration on shutdown
	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);

	// Component must be attached to a Player node
	main_player = checked_ptr_cast<Player>(node);
	if (!main_player)
		Log::error("CameraTerrainFetchSample::init(): CameraTerrainFetchSample must be assigned to a player node!\n");
}

// Async terrain fetch is initiated or results are displayed each frame.
void CameraTerrainFetchSample::update()
{
	if (!main_player)
		return;

	// Ray endpoints are calculated from mouse cursor position
	Vec3 a, b;
	auto main_window = WindowManager::getMainWindow();
	auto mouse = Input::getMousePosition();

	main_player->getDirectionFromMainWindow(a, b, mouse.x, mouse.y);

	// Fetch object is created on first frame
	if (!fetch)
	{
		fetch = LandscapeFetch::create();

		// Data types to fetch from terrain are configured
		fetch->setUsesHeight(true);
		fetch->setUsesNormal(true);
		fetch->setUsesAlbedo(true);

		// All mask channels are enabled for fetching
		for (int i = 0; i < num_masks; i += 1)
			fetch->setUsesMask(i, true);

		// Initial async intersection is queued
		fetch->intersectionAsync(a, a + ((b - a) * Game::getPlayer()->getZFar()));
	}

	else
	{
		// Results are processed when async operation completes
		if (fetch->isAsyncCompleted())
		{
			if (fetch->isIntersection())
			{
				// Terrain data is retrieved from fetch result
				auto terrain = Landscape::getActiveTerrain();
				auto point = fetch->getPosition();
				auto height = fetch->getHeight();
				auto vector_length = 10.f;
				StringStack<> info;

				// Info string is built with height and mask values
				info += String::format("height : %.3f\n", height);
				info += "masks: \n";

				for (int i = 0; i < num_masks; i++)
					info += String::format("    %s: %.2f\n", terrain->getDetailMask(i)->getName(), fetch->getMask(i));

				// Normal vector, sphere, and text are rendered at intersection point
				Visualizer::renderVector(point, point + Vec3(fetch->getNormal() * vector_length), vec4_white, .25f);
				Visualizer::renderSolidSphere(1.f, translate(point), vec4_white);
				Visualizer::renderMessage3D(point, vec3(1.f, 1.f, 0.f), info.get(), vec4_green, 1);
			}

			else
				// Error message is shown when ray misses terrain
				Visualizer::renderMessage3D(b, vec3(1.f, 1.f, 0), "Out of terrain.", vec4_red, 1);

			// Next async intersection is queued
			fetch->intersectionAsync(a, a + ((b - a) * Game::getPlayer()->getZFar()));
		}
	}
}

void CameraTerrainFetchSample::shutdown()
{
	Visualizer::setEnabled(visualizer_enabled);
	fetch = nullptr;
}
