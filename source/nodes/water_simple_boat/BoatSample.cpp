#include "BoatSample.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(BoatSample);

using namespace Unigine;
using namespace Math;

ConsoleVariableFloat global_lifetime("global_lifetime", "global_lifetime", 0, 1.0f, 0.0f, 100.0f);

void BoatSample::init()
{
	auto water_object = checked_ptr_cast<ObjectWaterGlobal>(water.get());
	if (water_object)
	{
		water_object->setFetchSteepnessQuality(ObjectWaterGlobal::STEEPNESS_QUALITY_HIGH);
		water_object->setFetchAmplitudeThreshold(0.001f);
	}

	Console::setFloat("global_buoyancy", 1.0f);

	sample_description_window.createWindow();

	sample_description_window.addFloatParameter("Beaufort", "", 4, 1, 8, [this](float value) {
		auto water_object = checked_ptr_cast<ObjectWaterGlobal>(water.get());
		if (water_object)
		{
			water_object->setBeaufort(value);
		}
	});

	sample_description_window.addFloatParameter("Particles Life Time", "", 1, 0, 10,
		[](float v) { global_lifetime.set(v); });
}

void BoatSample::shutdown()
{
	sample_description_window.shutdown();
}
