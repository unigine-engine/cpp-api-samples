// Fixed Function Pipeline (FFP) rendering example. Draws a rotating colored
// fan using immediate-mode triangle rendering. FFP provides simple 2D/3D
// drawing without custom shaders, useful for debug visualization and UI.

#include "FFPSample.h"

#include <UnigineFfp.h>
#include <UnigineGame.h>

using namespace Unigine;
using namespace Unigine::Math;

REGISTER_COMPONENT(FFPSample)

// Subscribes to post-GUI render event to draw FFP geometry after UI.
void FFPSample::init()
{
	// Subscribe to the "end of GUI rendering" event
	// so our render() function is called after the engine finishes rendering.
	Engine::get()->getEventEndPluginsGui().connect(*this, &FFPSample::render);
}

// Draws a rotating colored fan using FFP. Sets up orthographic projection,
// creates a triangle fan from 16 vertices with per-vertex colors.
void FFPSample::render()
{
	Ffp::setTextureSample(1);
	const EngineWindowViewportPtr window = WindowManager::getMainWindow();
	const auto time = Game::getTime();

	// Screen size
	const int width = window->getClientRenderSize().x;
	const int height = window->getClientRenderSize().y;
	const float radius = height / 2.0f;

	Ffp::enable(Ffp::MODE_SOLID);
	Ffp::setOrtho(width, height);

	// Begin rendering triangles
	// We should specify primitives and vertex data between
	// beginTriangles() and endTriangles()
	Ffp::beginTriangles();

	// Vertex colors
	constexpr unsigned int colors[] = {0xffff0000, 0xff00ff00, 0xff0000ff};

	// Create vertices
	constexpr int num_vertex = 16;
	for (int i = 0; i < num_vertex; i++)
	{
		const float angle = Consts::PI2 * i / (num_vertex - 1) - time;
		const float x = width / 2 + sinf(angle) * radius;
		const float y = height / 2 + cosf(angle) * radius;
		Ffp::addVertex(x, y);
		Ffp::setColor(colors[i % 3]);
	}

	// Create indices
	for (int i = 1; i < num_vertex; i++)
	{
		Ffp::addIndex(0);
		Ffp::addIndex(i);
		Ffp::addIndex(i - 1);
	}

	// End rendering triangles
	Ffp::endTriangles();

	// Disable FFP rendering
	Ffp::disable();
}
