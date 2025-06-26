#include "IFpsMovementSample.h"

REGISTER_COMPONENT(IFpsMovementSample);

using namespace Unigine;
using namespace Math;


void IFpsMovementSample::init()
{
	window_sample.createWindow();

	WidgetSliderPtr fpsSlider = window_sample.addIntParameter("Max FPS", "Max FPS", 60, 15, 150, [this](int v) {
		Render::setMaxFPS(v);
		});
	Render::setMaxFPS(60);
}

void IFpsMovementSample::shutdown()
{
	window_sample.shutdown();
}
