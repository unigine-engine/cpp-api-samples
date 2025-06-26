#include "CraneRopeSample.h"

#include "SlingRope.h"

using namespace Unigine;
using namespace Math;

REGISTER_COMPONENT(CraneRopeSample);

void CraneRopeSample::init()
{
	sample_description_window.createWindow();

	rope_component = ComponentSystem::get()->getComponentInWorld<SlingRope>(true);
	if (!rope_component)
		Log::error("%s: can't fine SlingRope component in world!\n", __FUNCTION__);

	sample_description_window.addIntParameter("Bag mass (kg)", "Bag mass (kg)", 5, 1, 50, [this](int value) {
		if (rope_component)
			rope_component->setBagMass(value);
	});

	sample_description_window.addBoolParameter("Visualizer", "Enable/disable debug mode", false,
		[this](bool value) {
			if (rope_component)
				rope_component->setDebugEnabled(value);
		});

	sample_description_window.addBoolParameter("Compensate Tension", "Enable/disable tension compensation",
		false, [this](bool value) {
			if (rope_component)
				rope_component->setTensionCompensationEnabled(value);
		});
}

void CraneRopeSample::shutdown()
{
	sample_description_window.shutdown();
}
