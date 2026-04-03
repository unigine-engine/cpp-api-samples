// Spring variant using intuitive parameters (duration, damping_ratio) instead of
// raw physics values. Converts to stiffness/damping using damped harmonic oscillator
// settling time formulas for predictable animation timing.

#include "SpringEasy.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(SpringEasy);

using namespace Unigine;
using namespace Math;

// Stiffness and damping are calculated from intuitive parameters using harmonic oscillator formulas.
void SpringEasy::refresh_spring_settings()
{
	// Convert intuitive parameters (duration, damping_ratio) to physics parameters.
	// Derived from the damped harmonic oscillator settling time formula.
	float frac_log = Math::log(fraction_of_amplitude);
	stiffness = Math::pow(frac_log, 2) / (Math::pow(duration, 2) * Math::pow(damping_ratio, 2));
	// Critical damping coefficient: 2 * sqrt(k) * zeta
	damping = 2.0f * sqrtf(stiffness) * damping_ratio;
}