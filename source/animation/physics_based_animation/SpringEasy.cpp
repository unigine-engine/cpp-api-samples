#include "SpringEasy.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(SpringEasy);

using namespace Unigine;
using namespace Math;

void SpringEasy::refresh_spring_settings()
{
	float frac_log = Math::log(fraction_of_amplitude);
	stiffness = Math::pow(frac_log, 2) / (Math::pow(duration, 2) * Math::pow(damping_ratio, 2));
	damping = 2.0f * sqrtf(stiffness) * damping_ratio;
}