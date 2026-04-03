#pragma once

#include "SpringMotion.h"

#include <UnigineComponentSystem.h>

// Simplified spring configuration using intuitive parameters.
// Instead of raw stiffness/damping values, uses:
// - duration: time for spring to settle
// - damping_ratio: oscillation amount (0.01-1, lower = more bouncy)
// Stiffness and damping are computed automatically from these values.
class SpringEasy : public SpringMotion

{
public:
	COMPONENT_DEFINE(SpringEasy, SpringMotion);

	PROP_PARAM(Float, duration, 6.0f);
	// Damping ratio [0.01, 1]
	PROP_PARAM(Float, damping_ratio, 0.6f);
	// A spring never gets to 0 amplitude, it gets infinitely smaller.
	// This fraction represents the perceived 0 point.
	PROP_PARAM(Float, fraction_of_amplitude, 1500.0f);

private:
	void refresh_spring_settings() override;
};
