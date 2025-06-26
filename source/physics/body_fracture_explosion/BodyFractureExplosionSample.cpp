#include "BodyFractureExplosionSample.h"
#include "BodyFractureExplosion.h"
#include <UnigineConsole.h>

REGISTER_COMPONENT(BodyFractureExplosionSample);

using namespace Unigine;

void BodyFractureExplosionSample::init()
{
	sample_description_window.createWindow();

	const auto& paramBox = sample_description_window.getParameterGroupBox();
	auto btn = WidgetButton::create("Explode!");
	btn->getEventClicked().connect(*this, [this]() {
		auto explosion = getComponent<BodyFractureExplosion>(target);
		if (explosion)
			explosion->explode();
	});
	paramBox->addChild(btn);
}

void BodyFractureExplosionSample::shutdown()
{
	sample_description_window.shutdown();
}
