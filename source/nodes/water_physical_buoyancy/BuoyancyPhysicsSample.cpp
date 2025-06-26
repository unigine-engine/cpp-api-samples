#include "BuoyancyPhysicsSample.h"

REGISTER_COMPONENT(BuoyancyPhysicsSample);

using namespace Unigine;

void BuoyancyPhysicsSample::init()
{
	sample_description_window.createWindow();

	waves = getComponent<Waves>(waves_node.get());
	sample_description_window.addFloatParameter("Beaufort", "beaufort", 0.f, 0.f, 8.f,
		[this](float v) { waves->setBeaufort(v); });
}

void BuoyancyPhysicsSample::shutdown()
{
	sample_description_window.shutdown();
}
