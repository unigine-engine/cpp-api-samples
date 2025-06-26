#include "CameraTerrainFetchSample.h"

#include <UnigineVisualizer.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(CameraTerrainFetchSample)

void CameraTerrainFetchSample::init()
{
	visualizer_enabled = Visualizer::isEnabled();
	Visualizer::setEnabled(true);

	main_player = checked_ptr_cast<Player>(node);
	if (!main_player)
		Log::error("CameraTerrainFetchSample::init(): CameraTerrainFetchSample must be assigned to a player node!\n");
}

void CameraTerrainFetchSample::update()
{
	if (!main_player)
		return;

	Vec3 a, b;
	auto main_window = WindowManager::getMainWindow();
	auto mouse = Input::getMousePosition();

	main_player->getDirectionFromMainWindow(a, b, mouse.x, mouse.y);

	if (!fetch)
	{
		fetch = LandscapeFetch::create();

		fetch->setUsesHeight(true);
		fetch->setUsesNormal(true);
		fetch->setUsesAlbedo(true);

		for (int i = 0; i < num_masks; i += 1)
			fetch->setUsesMask(i, true);

		fetch->intersectionAsync(a, a + ((b - a) * Game::getPlayer()->getZFar()));
	}

	else
	{
		if (fetch->isAsyncCompleted())
		{
			if (fetch->isIntersection())
			{
				auto terrain = Landscape::getActiveTerrain();
				auto point = fetch->getPosition();
				auto height = fetch->getHeight();
				auto vector_length = 10.f;
				StringStack<> info;

				info += String::format("height : %.3f\n", height);
				info += "masks: \n";

				for (int i = 0; i < num_masks; i++)
					info += String::format("    %s: %.2f\n", terrain->getDetailMask(i)->getName(), fetch->getMask(i));

				Visualizer::renderVector(point, point + Vec3(fetch->getNormal() * vector_length), vec4_white, .25f);
				Visualizer::renderSolidSphere(1.f, translate(point), vec4_white);
				Visualizer::renderMessage3D(point, vec3(1.f, 1.f, 0.f), info.get(), vec4_green, 1);
			}

			else
				Visualizer::renderMessage3D(b, vec3(1.f, 1.f, 0), "Out of terrain.", vec4_red, 1);

			fetch->intersectionAsync(a, a + ((b - a) * Game::getPlayer()->getZFar()));
		}
	}
}

void CameraTerrainFetchSample::shutdown()
{
	Visualizer::setEnabled(visualizer_enabled);
	fetch = nullptr;
}
