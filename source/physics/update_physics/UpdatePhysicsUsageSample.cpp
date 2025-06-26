#include "UpdatePhysicsUsageSample.h"

REGISTER_COMPONENT(UpdatePhysicsUsageSample);

using namespace Unigine;
using namespace Math;


void UpdatePhysicsUsageSample::init()
{
	window_sample.createWindow();

	WidgetSliderPtr fpsSlider = window_sample.addIntParameter("Max fps", "Max fps", 60, 15, 150, [this](int v) {
		Render::setMaxFPS(v);
		});
	Render::setMaxFPS(60);

}

void UpdatePhysicsUsageSample::shutdown()
{
	window_sample.shutdown();
}
