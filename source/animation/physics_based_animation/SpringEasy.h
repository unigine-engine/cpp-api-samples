#pragma once

#include "SpringMotion.h"

#include <UnigineComponentSystem.h>

class SpringEasy : public SpringMotion

{
public:
	COMPONENT_DEFINE(SpringEasy, SpringMotion);

	PROP_PARAM(Float, duration, 6.0f);
	// [0.01, 1]
	PROP_PARAM(Float, damping_ratio, 0.6f);
	// a spring never gets to 0 amplitude, it gets infinitely smaller. This fraction represents the
	// perceived 0 point
	PROP_PARAM(Float, fraction_of_amplitude, 1500.0f);

private:
	void refresh_spring_settings() override;
};
