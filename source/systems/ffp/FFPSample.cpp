#include "FFPSample.h"

#include <UnigineFfp.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Unigine::Math;

REGISTER_COMPONENT(FFPSample)

void FFPSample::init()
{

	// we want to render after engine finished render
	Engine::get()->getEventEndPluginsGui().connect(*this, &FFPSample::render);
}

void FFPSample::render()
{
	Ffp::setTextureSample(1);
	const EngineWindowViewportPtr window = WindowManager::getMainWindow();
	const auto time = Game::getTime();
	// screen size
	const int width = window->getClientRenderSize().x;
	const int height = window->getClientRenderSize().y;
	const float radius = height / 2.0f;

	Ffp::enable(Ffp::MODE_SOLID);
	Ffp::setOrtho(width, height);

	// begin triangles
	Ffp::beginTriangles();

	// vertex colors
	constexpr unsigned int colors[] = {0xffff0000, 0xff00ff00, 0xff0000ff};

	// create vertices
	constexpr int num_vertex = 16;
	for (int i = 0; i < num_vertex; i++)
	{
		const float angle = Consts::PI2 * i / (num_vertex - 1) - time;
		const float x = width / 2 + sinf(angle) * radius;
		const float y = height / 2 + cosf(angle) * radius;
		Ffp::addVertex(x, y);
		Ffp::setColor(colors[i % 3]);
	}

	// create indices
	for (int i = 1; i < num_vertex; i++)
	{
		Ffp::addIndex(0);
		Ffp::addIndex(i);
		Ffp::addIndex(i - 1);
	}

	// end triangles
	Ffp::endTriangles();

	Ffp::disable();
}
