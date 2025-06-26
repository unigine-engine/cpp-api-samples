#include "BuoySample.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(BuoySample);

using namespace Unigine;
using namespace Math;


ConsoleVariableFloat global_buoyancy("global_buoyancy", "global_buoyancy", 0, 1.0f, 0.0f, 100.0f);

void BuoySample::init()
{
	auto water_object = checked_ptr_cast<ObjectWaterGlobal>(water.get());
	if (water_object)
	{
		water_object->setFetchSteepnessQuality(ObjectWaterGlobal::STEEPNESS_QUALITY_HIGH);
		water_object->setFetchAmplitudeThreshold(0.001f);
	}

	sample_description_window.createWindow();

	std::function<void(float)> callback_ = [this](float new_value) {
		Console::setFloat("global_buoyancy", new_value);
	};

	sample_description_window.addFloatParameter("Buoyancy", "Change buoyancy", 0.4f, 0.f, 1.0f, callback_);

	water_object->setBeaufort(0.4f);

	callback_ = [this](float new_value) {
		auto water_object = checked_ptr_cast<ObjectWaterGlobal>(water.get());
		if (water_object)
			water_object->setBeaufort(new_value);
	};

	sample_description_window.addFloatParameter("Beaufort", "Change beaufort", 0.f, 0.f, 12.f, callback_);
}

void BuoySample::shutdown()
{
	sample_description_window.shutdown();
}
