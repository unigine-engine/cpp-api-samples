// Sample UI for demonstrating frame-rate independent movement. Slider controls
// Render::setMaxFPS to artificially vary frame rate, showing the difference
// between IFps-scaled and raw per-frame movement.

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
