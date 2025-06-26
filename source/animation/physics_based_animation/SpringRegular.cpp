#include "SpringRegular.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(SpringRegular);

using namespace Unigine;
using namespace Math;

void SpringRegular::RefreshSpring()
{
	refresh_spring_settings();
}

void SpringRegular::on_disable()
{
	refresh_spring_settings();
}

void SpringRegular::refresh_spring_settings()
{
	stiffness = defaultStiffness;
	damping = defaultDamping;
}