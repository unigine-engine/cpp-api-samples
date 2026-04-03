// Sample UI for adjusting framerate to observe the difference between
// applying forces in update() vs update_physics(). Lower FPS makes the
// instability of update()-based physics more visible.

#include "UpdatePhysicsUsageSample.h"

REGISTER_COMPONENT(UpdatePhysicsUsageSample);

using namespace Unigine;
using namespace Math;


void UpdatePhysicsUsageSample::init()
{
	window_sample.createWindow();

	// FPS slider demonstrates physics stability at different render rates
	WidgetSliderPtr fpsSlider = window_sample.addIntParameter("Max fps", "Max fps", 60, 15, 150, [this](int v) {
		Render::setMaxFPS(v);
		});
	Render::setMaxFPS(60);

}

void UpdatePhysicsUsageSample::shutdown()
{
	window_sample.shutdown();
}
